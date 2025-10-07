# Krallık Sistemi Değişiklikleri

Bu dosya, oyunun eski 3 krallık sisteminden (kırmızı, mavi, sarı) yeni özelleştirilebilir krallık sistemine geçiş için yapılan değişiklikleri açıklar.

## Değişiklik Özeti

### Eski Sistem
- Sabit 3 krallık (A, B, C - kırmızı, mavi, sarı)
- Oyuncular sadece mevcut krallıklardan birini seçebiliyordu
- Krallık yönetimi yoktu
- Krallık özelleştirmesi yoktu

### Yeni Sistem
- Oyuncular kendi krallıklarını oluşturabilir
- Krallık adı, rengi ve bayrağı özelleştirilebilir
- Krallık yönetim sistemi (davet, atma, rütbe verme)
- Genshin Impact tarzı kişisel land sistemi

## Değiştirilen Dosyalar

### Client Side (Binary/root/)

1. **introempire_new.py** (YENİ)
   - Eski SelectEmpireWindow yerine CreateKingdomWindow ve SelectKingdomWindow
   - Krallık oluşturma ve seçme arayüzleri

2. **networkmodule.py** (DEĞİŞTİRİLDİ)
   - Krallık sistemi için yeni network fonksiyonları eklendi
   - SetSelectEmpirePhase fonksiyonu güncellendi
   - Encoding UTF-8'e çevrildi

3. **uikingdom.py** (YENİ)
   - Krallık yönetim arayüzü
   - Üye yönetimi, ayarlar, davet sistemi

4. **kingdom_network.py** (YENİ)
   - Network fonksiyonları ve packet header'ları

### Server Side (Serverside/Server/game/src/)

1. **kingdom.h** (YENİ)
   - Krallık sistemi için C++ header dosyası
   - Struct'lar, enum'lar ve packet tanımları

2. **kingdom.cpp** (YENİ)
   - Krallık yönetim sistemi implementasyonu
   - CKingdomManager sınıfı

### Database

1. **kingdom_database.sql** (YENİ)
   - Yeni veritabanı tabloları
   - Örnek veriler

## Yeni Özellikler

### Krallık Oluşturma
- Oyuncular kendi krallıklarını oluşturabilir
- Krallık adı (3-20 karakter)
- RGB renk seçimi
- 5 farklı bayrak tasarımı

### Krallık Yönetimi
- **Rütbeler**: Üye, Subay, Komutan, Kral
- **Yetkiler**: 
  - Kral: Tüm yetkiler
  - Komutan: Üye davet etme, atma, subay yapma
  - Subay: Üye davet etme
  - Üye: Temel üyelik

### Krallık Özellikleri
- Üye listesi görüntüleme
- Üye davet etme
- Üye atma
- Rütbe verme/alma
- Krallık ayarları (ad, renk, açıklama)
- Krallıktan ayrılma

## Kurulum Talimatları

### 1. Veritabanı Kurulumu
```sql
-- kingdom_database.sql dosyasını veritabanına import edin
mysql -u root -p oyun_db < kingdom_database.sql
```

### 2. Server Kurulumu
```cpp
// Aşağıdaki dosyaları server projesine ekleyin:
// - kingdom.h
// - kingdom.cpp

// main.cpp veya uygun yerde initialize edin:
CKingdomManager::instance().LoadKingdomsFromDB();
```

### 3. Client Kurulumu
- Yeni Python dosyalarını Binary/root/ dizinine kopyalayın
- networkmodule.py dosyasını güncelleyin
- UI script dosyalarını gerekirse oluşturun

## Packet Protokolü

### Client to Server
- `HEADER_CG_CREATE_KINGDOM` (200): Krallık oluştur
- `HEADER_CG_JOIN_KINGDOM` (201): Krallığa katıl
- `HEADER_CG_LEAVE_KINGDOM` (202): Krallıktan ayrıl
- `HEADER_CG_REQUEST_KINGDOM_LIST` (203): Krallık listesi iste
- `HEADER_CG_KINGDOM_INVITE` (204): Oyuncu davet et
- `HEADER_CG_KINGDOM_KICK` (205): Oyuncu at
- `HEADER_CG_KINGDOM_RANK` (206): Rütbe değiştir
- `HEADER_CG_KINGDOM_SETTINGS` (207): Krallık ayarları

### Server to Client
- `HEADER_GC_KINGDOM_LIST` (200): Krallık listesi
- `HEADER_GC_KINGDOM_INFO` (201): Krallık bilgileri
- `HEADER_GC_KINGDOM_MEMBER_LIST` (202): Üye listesi
- `HEADER_GC_KINGDOM_INVITE` (203): Davet bildirimi
- `HEADER_GC_KINGDOM_JOIN_RESULT` (204): Katılma sonucu
- `HEADER_GC_KINGDOM_LEAVE_RESULT` (205): Ayrılma sonucu

## Test Senaryoları

1. **Krallık Oluşturma**
   - Yeni oyuncu krallık oluşturabilmeli
   - Aynı isimde krallık oluşturulamamalı
   - Oluşturan oyuncu otomatik kral olmalı

2. **Krallığa Katılma**
   - Mevcut krallıklara katılabilmeli
   - Zaten krallığı olan oyuncu katılamamalı
   - Davet sistemi çalışmalı

3. **Üye Yönetimi**
   - Yetki kontrolü çalışmalı
   - Rütbe sistemi doğru çalışmalı
   - Kral değişimi otomatik olmalı

## Gelecek Geliştirmeler

- Krallık savaşları
- Krallık binaları
- Krallık hazinesi
- Krallık görevleri
- Krallık diplomasisi
- Land sistemi genişletmesi

## Notlar

- Eski empire sistemi tamamen kaldırıldı
- Mevcut karakter verileri etkilenmez
- Veritabanı migration gerekebilir
- UI script dosyaları oluşturulması gerekebilir
