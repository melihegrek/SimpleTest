#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <string>
#include <mutex>
#include <tinyxml2.h>

// Yapılandırmaları tutmak için bir struct tanımlayalım
struct ThreadConfig {
    int priority;
    int target;
};

class Config {
public:
    // Singleton yapısı için getInstance metodu
    static Config& getInstance();

    // Yapılandırmayı yüklemek için metot
    bool loadConfig();

    // Thread bilgilerini almak için metot
    const std::vector<ThreadConfig>& getThreads() const;

    // Kopyalama ve atama operatörlerini devre dışı bırak
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

private:
    Config();


    // Thread bilgilerini parse eden yardımcı metot
    void parseThreads(tinyxml2::XMLElement* root);

    std::string m_filename; // XML dosyasının adı
    std::vector<ThreadConfig> m_threads; // Thread yapılandırmalarını tutan vektör
    std::mutex m_mutex; // Eş zamanlılık için mutex
};

#endif // CONFIG_H
