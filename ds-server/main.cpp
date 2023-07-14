#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <grpcpp/grpcpp.h>
#include "diskspeed.grpc.pb.h"
#include "diskspeed.pb.h"

class DiskSpeedServiceImpl final : public diskspeed::DiskSpeedService::Service {
public:
	grpc::Status MeasureSpeed(grpc::ServerContext* context, const diskspeed::SpeedRequest* request,
							  diskspeed::SpeedResponse* response) override {
		std::string file_name = request->file_name();
		int32_t file_size = request->file_size();

		std::ofstream testFile(file_name);
		char* buffer = new char[file_size];
		testFile.write(buffer, file_size);
		delete[] buffer;
		testFile.close();
		double read_speed = MeasureReadSpeed(file_name, file_size);
		double write_speed = MeasureWriteSpeed(file_name, file_size);
		response->set_read_speed(read_speed);
		response->set_write_speed(write_speed);
		std::remove(file_name.c_str());
		return grpc::Status::OK;
	}
private:
	double MeasureReadSpeed(const std::string& file_name, int32_t file_size) {
		std::ifstream inputFile(file_name, std::ios::binary);
		char* read_buffer = new char [file_size];
		auto start = std::chrono::steady_clock::now();
		inputFile.read(read_buffer, file_size);
		auto end = std::chrono::steady_clock::now();
		inputFile.close();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		double read_speed = (file_size / (1024.0 * 1024.0)) / (duration / 1000.0);
		delete [] read_buffer;
		return read_speed;
	}

	double MeasureWriteSpeed(const std::string& file_name, int32_t file_size) {
		std::ofstream outputFile(file_name, std::ios::binary);
		char* write_buffer = new char [file_size];
		auto start = std::chrono::steady_clock::now();
		outputFile.write(write_buffer, file_size);
		auto end = std::chrono::steady_clock::now();
		outputFile.close();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count();
		double write_speed = (file_size / (1024.0 * 1024.0)) / (duration / 1000.0);
		delete [] write_buffer;
		return write_speed;
	}
};

void RunServer() {
	std::string server_address = "0.0.0.0:50051";
	DiskSpeedServiceImpl service;

	grpc::ServerBuilder builder;
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
	std::cout<< "Server Su Adreste Baslatildi "<<server_address<<std::endl;
	server->Wait();
}

int main() {
	RunServer();
	return 0;
}
