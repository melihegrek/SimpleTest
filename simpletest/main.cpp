#include <iostream>
#include <sstream> 

#include <vector>
#include <thread>
#include <csignal>
#include <chrono>
#include <iomanip>
#include "config.h"
#include "logger.h"


/* ========== VARIABLES ========== */
std::vector<std::thread> threads;  // Tüm iş parçacıklarını saklamak için bir vektör

/* ========== FUNCTIONS ========== */
void parseConfigXML();
void threadTaskFunc(int priority, int target); // İş parçacığı işlevi
void clearFile(const std::string&);
// Global değişken
bool isFirstWrite = true;

int main() {

    sched_param sch_params;
    sch_params.sched_priority = 99; /* MAX = 99 */
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &sch_params);

    // Start logger
    Logger& logger = Logger::getInstance();
    logger.start();
    logger.log("main", "Logger is running");


    // Load configuration
    Config& config = Config::getInstance();
    if (!config.loadConfig()) {
        logger.log("main", "Failed to load config!");
        return 1;
    }

    // Parse configuration and create threads
    parseConfigXML();

    // Wait for all threads to complete
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    // Stop logger
    logger.stop();

    return 0;
}

void parseConfigXML() {
    const auto& threadsConfig = Config::getInstance().getThreads(); // Konfigürasyon dosyasındaki thread bilgilerini al
        //std::cout << "Vector length: " << threads.size() << std::endl;
    for (const auto& threadConfig : threadsConfig) {
        std::cout << "Creating thread with priority " << threadConfig.priority 
                  << " and target " << threadConfig.target << std::endl;
        
        // Her bir thread için uygun iş parçacığı fonksiyonunu başlat
        threads.emplace_back(threadTaskFunc, threadConfig.priority, threadConfig.target);
    }
}

void threadTaskFunc(int priority, int target) {
    Logger& logger = Logger::getInstance();    
    
    // Thread başlangıç zamanını al
    auto start = std::chrono::high_resolution_clock::now();
    auto startEpochMicro = std::chrono::duration_cast<std::chrono::microseconds>(start.time_since_epoch()).count();

    // Thread başlangıcını logla
    logger.log("threadTaskFunc", "Thread started at epoch time " + std::to_string(startEpochMicro) + " microseconds with priority " + std::to_string(priority));
    
    // Thread önceliğini ayarla
    sched_param sch_params;
    sch_params.sched_priority = priority;
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &sch_params);

    // Thread görevi: hedef sayıya kadar çarpma işlemi
    for (int i = 1; i <= target; ++i) {
        // std::cout << "Thread " << std::this_thread::get_id() << " multiplying: " << i << " * " << target << " = " << i * target << std::endl;
    }

    // Thread bitiş zamanını al
    auto end = std::chrono::high_resolution_clock::now();
    
    // Mikro saniye hassasiyetle epoch zamanı elde et
    auto endEpochMicro = std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch()).count();

    // Thread bitişini logla
    logger.log("threadTaskFunc", "Thread with priority " + std::to_string(priority) + " ended at epoch time " + std::to_string(endEpochMicro) + " microseconds");

    // Thread süresini logla
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    logger.log("threadTaskFunc", "Thread duration: " + std::to_string(duration) + " milliseconds");
    
    // CSV dosyasına verileri yaz
    std::ofstream outFile("record.csv", std::ios::app);
    if (outFile.is_open()) {
        // İlk yazmada başlıkları ekle
        if (isFirstWrite) {
            clearFile("record.csv"); // Dosyayı temizle
            outFile << "Priority,StartEpoch,EndEpoch\n";
            isFirstWrite = false;
        }
        outFile << priority << "," << startEpochMicro << "," << endEpochMicro << "\n";
        outFile.close();
    } else {
        std::cerr << "Unable to open file record.csv";
    }
}
 void clearFile(const std::string& filename) {
    // Dosyayı aç, içeriğini temizle ve kapat
    std::ofstream outFile(filename, std::ios::out | std::ios::trunc);
    if (!outFile) {
        std::cerr << "Unable to open file " << filename << " for clearing.";
    }
}
