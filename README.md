# Hard-Disk-Read-Write-Speed-With-gRPC-Microservice
Projede bilgisayar üzerindeki sabit diskin verileri okuma ve yazma hızları hesaplanmıştır. Burada bu sonuç şu şekilde elde edilmiştir. Server tarafında örnek bir "test.txt" dosyasına 100 MB boyutunda veri aktarılıp bu verilerin aktarımında ve okunmasında geçen süre hesaplanmıştır. Dosya verileri boyutu kadar geçici bir buffer üzerinde depolanmıştır. Ardından aktarılan veri boyutu aktarım sırasında geçen süreye bölünerek okuma ve yazma hızları hesaplanmıştır. Bunun ardından oluşturulan lokalde bağlı gRPC kanalı ile client tarafına aktarmaya hazır konuma gelmiştir.
Client tarafında ise yine bir gRPC kanalı kurulmuş ve server tarafına dosya boyutu ve dosya adı atılmıştır. Atılan bu isteğin gelen cevapları ise sabit diskin okuma ve yazma hızları olarak gelmiştir. 
Burada bu veri aktarımı worker thread tarafında çalışmıştır. 
Verilerin sürekli olarak aktarılabilmesi için arayüzün sürekli olarak güncellenmesi sağlanmıştır. 
Son olarak arayüzde yer alan textEditlere okuma ve yazma hızları aktarılmıştır.
Not: gRPC kütüphanesinin kullanımı Qt üzerinde kurulu vcpkg üzerinde yapıldığı için Qt creator kullanmak zorunda kaldım.
