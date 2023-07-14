#include <iostream>
#include <grpcpp/grpcpp.h>
#include <thread>
#include "diskspeed.grpc.pb.h"
#include "form.h"
#include "ui_form.h"
#include <QApplication>
#include <QThread>
#include <QTimer>
#include <QMetaObject>

class SpeedUpdateEvent : public QEvent {
public:
	SpeedUpdateEvent(double readSpeed, double writeSpeed)
		: QEvent(QEvent::User), readSpeed_(readSpeed), writeSpeed_(writeSpeed) {}

	double readSpeed() const {
		return readSpeed_;
	}

	double writeSpeed() const {
		return writeSpeed_;
	}

private:
	double readSpeed_;
	double writeSpeed_;
};

class DiskSpeedClient {
public:
	DiskSpeedClient(std::shared_ptr<grpc::Channel> channel)
		: stub_(diskspeed::DiskSpeedService::NewStub(channel)), measurement_count_(0) {}

	void MeasureSpeed(const std::string& file_name, int32_t file_size) {
		std::string server_address("localhost:50051");

		while (true) {
			std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
			std::unique_ptr<diskspeed::DiskSpeedService::Stub> stub(diskspeed::DiskSpeedService::NewStub(channel));

			diskspeed::SpeedRequest request;
			request.set_file_name(file_name);
			request.set_file_size(file_size);

			grpc::ClientContext context;

			grpc::Status status = stub->MeasureSpeed(&context, request, &response);

			if (status.ok()) {
				double readSpeed = response.read_speed();
				double writeSpeed = response.write_speed();

				UpdateUI(readSpeed, writeSpeed);
			} else {
				std::cout << "RPC hatasi: " << status.error_code() << ": " << status.error_message() << std::endl;
			}

			measurement_count_++;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}

	double readSpeed() {
		return response.read_speed();
	}

	double writeSpeed() {
		return response.write_speed();
	}

private:
	std::unique_ptr<diskspeed::DiskSpeedService::Stub> stub_;
	diskspeed::SpeedResponse response;
	int measurement_count_;

	void UpdateUI(double readSpeed, double writeSpeed) {
		QMetaObject::invokeMethod(qApp, [this, readSpeed, writeSpeed]() {
			QApplication::postEvent(qApp, new SpeedUpdateEvent(readSpeed, writeSpeed));
		});
	}
};

class SpeedUpdateHandler : public QObject {
public:
	SpeedUpdateHandler(Ui::Form* ui, Form* form)
			: QObject(nullptr), ui_(ui), form_(form) {}


	bool event(QEvent* event) override {
		while(form_->isRun()) {
		if (event->type() == QEvent::User) {
			SpeedUpdateEvent* speedEvent = static_cast<SpeedUpdateEvent*>(event);
			double readSpeed = speedEvent->readSpeed();
			double writeSpeed = speedEvent->writeSpeed();

			QString readSpeedStr = QString::number(readSpeed);
			QString writeSpeedStr = QString::number(writeSpeed);

			ui_->textEdit_readspeed->setText(readSpeedStr+ " MB/s");
			ui_->textEdit_writespeed->setText(writeSpeedStr+ " MB/s");

			return true;
		}
}
		return QObject::event(event);
	}


private:
	Ui::Form* ui_;
	Form* form_;
};

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	Form w;
	w.show();

	std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
	DiskSpeedClient client(channel);

	std::string file_name = "test.txt";
	int32_t file_size = 1024 * 1024 * 100;

	SpeedUpdateHandler updateHandler(w.getUI(),&w);
	app.installEventFilter(&updateHandler);

	std::thread grpcThread([&]() {
		client.MeasureSpeed(file_name, file_size);
	});

	QTimer updateTimer;
	QObject::connect(&updateTimer, &QTimer::timeout, [&]() {
		double readSpeed = client.readSpeed();
		double writeSpeed = client.writeSpeed();

		QMetaObject::invokeMethod(qApp, [&, readSpeed, writeSpeed]() {
			updateHandler.event(new SpeedUpdateEvent(readSpeed, writeSpeed));
		});
	});
	updateTimer.start(1000);

	int result = app.exec();

	grpcThread.join();

	return result;
}

