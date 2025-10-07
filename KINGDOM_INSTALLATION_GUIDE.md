# Krallık Sistemi Kurulum Rehberi

Bu rehber, yeni krallık sisteminin oyununuza nasıl kurulacağını adım adım açıklar.

## 1. Veritabanı Kurulumu

### Adım 1.1: SQL Dosyasını İçe Aktar
```bash
# MySQL'e bağlan
mysql -u root -p

# Oyun veritabanını seç
USE your_game_database;

# Kingdom tablolarını oluştur
SOURCE kingdom_database.sql;
```

### Adım 1.2: Tabloları Kontrol Et
```sql
-- Tabloların oluştuğunu kontrol et
SHOW TABLES LIKE 'kingdom%';

-- Örnek verileri kontrol et
SELECT * FROM kingdom;
SELECT * FROM kingdom_member;
```

## 2. Server Tarafı Kurulum

### Adım 2.1: Dosyaları Kopyala
```bash
# Kingdom sistem dosyalarını server src dizinine kopyala
cp Serverside/Server/game/src/kingdom.h /path/to/your/server/game/src/
cp Serverside/Server/game/src/kingdom.cpp /path/to/your/server/game/src/
cp Serverside/Server/game/src/kingdom_packet.h /path/to/your/server/game/src/
cp Serverside/Server/game/src/kingdom_packet.cpp /path/to/your/server/game/src/
```

### Adım 2.2: Makefile Güncelle
```makefile
# Makefile'ınıza şu satırları ekleyin:
SOURCES += kingdom.cpp kingdom_packet.cpp

# Veya mevcut SOURCES satırına ekleyin:
SOURCES = char.cpp char_manager.cpp ... kingdom.cpp kingdom_packet.cpp
```

### Adım 2.3: Header Include'ları Ekle

**main.cpp dosyasına ekleyin:**
```cpp
#include "kingdom.h"
#include "kingdom_packet.h"
```

**Initialization kısmına ekleyin:**
```cpp
// Main fonksiyonunda veya server başlatma kısmında
CKingdomManager::instance().LoadKingdomsFromDB();
```

### Adım 2.4: Packet Handler'ları Ekle

**input_main.cpp dosyasına ekleyin:**
```cpp
// Packet handler switch case'ine ekleyin:
case HEADER_CG_CREATE_KINGDOM:
    CKingdomPacketHandler::HandleCreateKingdom(ch, c_pData);
    break;

case HEADER_CG_JOIN_KINGDOM:
    CKingdomPacketHandler::HandleJoinKingdom(ch, c_pData);
    break;

case HEADER_CG_LEAVE_KINGDOM:
    CKingdomPacketHandler::HandleLeaveKingdom(ch, c_pData);
    break;

case HEADER_CG_REQUEST_KINGDOM_LIST:
    CKingdomPacketHandler::HandleRequestKingdomList(ch, c_pData);
    break;

case HEADER_CG_KINGDOM_INVITE:
    CKingdomPacketHandler::HandleKingdomInvite(ch, c_pData);
    break;

case HEADER_CG_KINGDOM_KICK:
    CKingdomPacketHandler::HandleKingdomKick(ch, c_pData);
    break;

case HEADER_CG_KINGDOM_RANK:
    CKingdomPacketHandler::HandleKingdomRank(ch, c_pData);
    break;

case HEADER_CG_KINGDOM_SETTINGS:
    CKingdomPacketHandler::HandleKingdomSettings(ch, c_pData);
    break;
```

### Adım 2.5: Server'ı Derle
```bash
cd /path/to/your/server/game/src
make clean
make
```

## 3. Client Tarafı Kurulum

### Adım 3.1: Python Dosyalarını Kopyala
```bash
# Root dizinine Python dosyalarını kopyala
cp Binary/root/introempire_new.py /path/to/your/client/Binary/root/
cp Binary/root/uikingdom.py /path/to/your/client/Binary/root/
cp Binary/root/kingdom_network.py /path/to/your/client/Binary/root/

# NetworkModule'ü güncelle
cp Binary/root/networkmodule.py /path/to/your/client/Binary/root/
```

### Adım 3.2: UI Script Dosyalarını Kopyala
```bash
# UI script dosyalarını kopyala
cp Binary/uiscript/uiscript/createkingdomwindow.py /path/to/your/client/Binary/uiscript/uiscript/
cp Binary/uiscript/uiscript/selectkingdomwindow.py /path/to/your/client/Binary/uiscript/uiscript/
cp Binary/uiscript/uiscript/kingdommanagementwindow.py /path/to/your/client/Binary/uiscript/uiscript/
cp Binary/uiscript/uiscript/kingdomsettingswindow.py /path/to/your/client/Binary/uiscript/uiscript/
```

### Adım 3.3: Game.py Entegrasyonu

**game.py dosyasına ekleyin:**
```python
# game_kingdom_integration.py dosyasındaki kodları game.py'ye entegre edin
# Bu dosya tüm gerekli fonksiyonları ve packet handler'ları içerir
```

### Adım 3.4: Locale Dosyalarını Güncelle

**uiScriptLocale.py dosyasına ekleyin:**
```python
# Kingdom system paths
KINGDOM_PATH = "d:/ymir work/ui/intro/kingdom/"

# Kingdom texts
KINGDOM_CREATE = "Krallık Oluştur"
KINGDOM_SELECT = "Krallık Seç"
KINGDOM_MANAGEMENT = "Krallık Yönetimi"
KINGDOM_SETTINGS = "Krallık Ayarları"
KINGDOM_INVITE = "Davet Et"
KINGDOM_KICK = "Üyeyi At"
KINGDOM_PROMOTE = "Terfi Ettir"
KINGDOM_DEMOTE = "Rütbe Düşür"
KINGDOM_LEAVE = "Ayrıl"
```

## 4. Test ve Doğrulama

### Adım 4.1: Server Testleri
1. Server'ı başlatın
2. Log dosyalarını kontrol edin
3. Kingdom tabloları yüklendiğini doğrulayın

### Adım 4.2: Client Testleri
1. Client'ı başlatın
2. Karakter seçim ekranında yeni krallık sistemi görünmeli
3. Krallık oluşturma/seçme işlevlerini test edin

### Adım 4.3: Fonksiyonel Testler
1. **Krallık Oluşturma:**
   - Yeni krallık oluşturun
   - Farklı renk ve bayrak seçeneklerini test edin

2. **Krallık Yönetimi:**
   - Oyuncu davet etmeyi test edin
   - Rütbe verme/alma işlemlerini test edin
   - Krallık ayarlarını değiştirmeyi test edin

3. **Krallık Üyeliği:**
   - Krallığa katılmayı test edin
   - Krallıktan ayrılmayı test edin

## 5. Sorun Giderme

### Yaygın Sorunlar

**Problem:** Server başlamıyor
**Çözüm:** 
- Derleme hatalarını kontrol edin
- MySQL bağlantısını doğrulayın
- Kingdom tablolarının var olduğunu kontrol edin

**Problem:** Client'ta krallık penceresi açılmıyor
**Çözüm:**
- Python dosyalarının doğru yerde olduğunu kontrol edin
- UI script dosyalarının var olduğunu kontrol edin
- Import hatalarını kontrol edin

**Problem:** Packet gönderimi çalışmıyor
**Çözüm:**
- Packet header'larının doğru tanımlandığını kontrol edin
- Network fonksiyonlarının doğru çağrıldığını kontrol edin
- Server'da packet handler'ların eklendiğini kontrol edin

### Log Kontrolleri

**Server log'larında arayın:**
```
Kingdom system initialized
Loading kingdoms from database
Kingdom loaded: [Kingdom Name]
```

**Client'ta hata mesajları:**
```python
# Python console'da hataları kontrol edin
import sys
print(sys.last_traceback)
```

## 6. Ek Özellikler

### Gelişmiş Konfigürasyon

**config.h dosyasına ekleyebilirsiniz:**
```cpp
// Kingdom system configuration
#define KINGDOM_MAX_MEMBERS 100
#define KINGDOM_NAME_MIN_LENGTH 3
#define KINGDOM_NAME_MAX_LENGTH 20
#define KINGDOM_DESCRIPTION_MAX_LENGTH 100
```

### Güvenlik Ayarları

**Ek güvenlik kontrolleri:**
```cpp
// Kingdom name validation
bool IsValidKingdomName(const std::string& name) {
    // Özel karakter kontrolü
    // Küfür filtresi
    // Uzunluk kontrolü
}
```

### Performans Optimizasyonu

**Veritabanı indeksleri:**
```sql
-- Performans için ek indeksler
CREATE INDEX idx_kingdom_name ON kingdom(name);
CREATE INDEX idx_kingdom_member_player ON kingdom_member(player_id);
CREATE INDEX idx_kingdom_member_kingdom ON kingdom_member(kingdom_id);
```

## 7. Bakım ve Güncelleme

### Düzenli Bakım
- Veritabanı backup'ları alın
- Log dosyalarını temizleyin
- Performans metrikleri takip edin

### Güncelleme Prosedürü
1. Server'ı durdurun
2. Veritabanı backup'ı alın
3. Yeni dosyaları kopyalayın
4. Server'ı yeniden derleyin
5. Test edin
6. Server'ı başlatın

Bu rehberi takip ederek krallık sistemini başarıyla kurabilirsiniz. Herhangi bir sorunla karşılaştığınızda, log dosyalarını kontrol edin ve adımları tekrar gözden geçirin.
