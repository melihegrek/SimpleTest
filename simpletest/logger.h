#ifndef LOGGER_H
#define LOGGER_H

#include <queue>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>

class Logger {
public:
    // Log mesajını temsil eden yapı
    struct LogMessage {
        std::string sender;
        std::string message;
        std::string dateTime;
    };

    // Singleton yapısı için getInstance metodu
    static Logger& getInstance();

    // Log mesajlarını eklemek için metodlar
    void log(const std::string& sender, const std::string& message);
    void log(const std::string& message);

    // Logger'ı başlatma ve durdurma
    void start();
    void stop();

    // Kopyalama ve atama operatörlerini devre dışı bırak
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger(); // Constructor
    ~Logger(); // Destructor

    // Log kuyruğundaki mesajları işlemek için metot
    void processQueue();

    // Geçerli tarih ve saati almak için yardımcı metot
    std::string getCurrentDateTime();

    // Log dosyasının ismini almak için yardımcı metot
    std::string getLogFileName();

    std::queue<LogMessage> logQueue; // Log mesajlarını tutan kuyruk
    std::mutex mtx; // Mutex, thread-safe erişim için
    std::condition_variable cv; // Koşul değişkeni
    bool stopFlag; // Logger'ın durdurulması için bayrak
    std::thread workerThread; // Log işleme iş parçacığı
    std::ofstream logFile; // Log dosyası
};

#endif // LOGGER_H
