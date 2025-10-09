# Sage Skill System - Perfect Sonrası Yeni Skill Sistemi

## Genel Bakış

Sage Skill Sistemi, mevcut Perfect skill seviyesinden sonra gelen ileri düzey bir skill geliştirme sistemidir.

### Skill Hiyerarşisi
```
Normal (1-17) → Master (M1-M10) → Grand Master (G1-G10) → Perfect → SAGE (S1-S10)
```

## Özellikler

### ✨ Temel Özellikler
- **10 Sage Seviyesi**: S1'den S10'a kadar progression
- **3 Yükseltme Yöntemi**: Antik Rulo, Bilge Taşı, Meditasyon
- **Güçlü Bonuslar**: Her seviyede artan hasar, cooldown ve mana bonusları
- **Özel Yetenekler**: Specific seviyelerde açılan unique abilities
- **Kingdom Entegrasyonu**: Krallık sistemi ile uyumlu requirements

### 🎯 Sage Bonusları
- **Hasar Bonusu**: Her seviyede +5% (S10'da toplam +50%)
- **Cooldown Azaltma**: Her seviyede -2% (S10'da toplam -20%)
- **Mana Azaltma**: Her seviyede -1.5% (S10'da toplam -15%)

### 🔮 Özel Yetenekler
- **S3**: Zincir Şimşek - Chain lightning attack
- **S5**: Çift Büyü - Double cast ability  
- **S7**: Mana Kalkanı - Mana shield protection
- **S9**: Zaman Manipülasyonu - Time manipulation
- **S10**: Gerçeklik Yırtığı - Reality tear ultimate

## Yükseltme Yöntemleri

### 🏺 Antik Rulo Yöntemi
- **Başarı Oranı**: 30% → 12% (kademeli azalma)
- **Cooldown**: 24 saat
- **Gereksinim**: Sınıfa özel Antik Rulo
- **Özellik**: Hızlı ama riskli

### 💎 Bilge Taşı Yöntemi  
- **Başarı Oranı**: 50% → 23% (kademeli azalma)
- **Cooldown**: 12 saat
- **Gereksinim**: Grade'e uygun Bilge Taşı
- **Özellik**: Daha yüksek başarı oranı

### 🧘 Meditasyon Yöntemi
- **Başarı Oranı**: 100% (Garantili)
- **Cooldown**: 6 saat
- **Gereksinim**: 30 dakika kesintisiz meditasyon
- **Özellik**: Garantili ama zaman alıcı

## Dosya Yapısı

### Server-side
```
Serverside/
├── sage_skill_system.h          # Header dosyası
├── sage_skill_system.cpp        # Ana implementasyon
└── sage_skill_database.sql      # Veritabanı şeması
```

### Client-side
```
Binary/
└── sage_skill_ui.py            # UI implementasyonu
```

### Documentation
```
├── SAGE_SKILL_README.md        # Bu dosya
└── SAGE_INSTALLATION_GUIDE.md  # Kurulum rehberi
```

## Kurulum

### 1. Veritabanı
```sql
mysql -u [user] -p [database] < sage_skill_database.sql
```

### 2. Server
```cpp
// main.cpp'ye ekleyin
#include "sage_skill_system.h"

void InitializeGame() {
    InitializeSageSkillSystem();
}
```

### 3. Client
```python
# game.py'ye ekleyin
import sage_skill_ui
```

## Chat Komutları

- `/sage_upgrade [skill_vnum]` - Skill yükseltme penceresi
- `/sage_info [skill_vnum]` - Sage skill bilgileri  
- `/sage_meditation [skill_vnum]` - Meditasyon başlatma

## Gerekli Items

### Antik Rulolar (Class-specific)
- Savaşçı Antik Rulosu (90001)
- Ninja Antik Rulosu (90002)
- Sura Antik Rulosu (90003)
- Şaman Antik Rulosu (90004)
- Kurt Adam Antik Rulosu (90005)

### Bilge Taşları (Grade-specific)
- Düşük Seviye Bilge Taşı (90010) - S1-S3
- Orta Seviye Bilge Taşı (90011) - S4-S6
- Yüksek Seviye Bilge Taşı (90012) - S7-S9
- Üstün Bilge Taşı (90013) - S10

## Teknik Detaylar

### Veritabanı Tabloları
- `sage_skills` - Ana sage skill verileri
- `sage_skill_upgrade_log` - Yükseltme geçmişi
- `sage_skill_cooldowns` - Cooldown yönetimi
- `sage_special_abilities` - Özel yetenekler
- `sage_meditation_sessions` - Meditasyon oturumları

### Network Packets
- `HEADER_CG_SAGE_SKILL_UPGRADE` - Yükseltme isteği
- `HEADER_CG_SAGE_SKILL_INFO` - Bilgi isteği
- `HEADER_CG_SAGE_MEDITATION_START` - Meditasyon başlatma
- `HEADER_GC_SAGE_SKILL_RESULT` - Sonuç paketi

## Özel Yetenekler Detayı

### 🌩️ Zincir Şimşek (S3)
- Hedefi ve yakındaki düşmanlara yıldırım hasarı
- Cooldown: 30 saniye
- Mana: 100
- Radius: 500

### ⚡ Çift Büyü (S5)  
- Sonraki büyünüzü iki kez kullanır
- Cooldown: 60 saniye
- Süre: 10 saniye
- Buff effect

### 🛡️ Mana Kalkanı (S7)
- Gelen hasarın %50'sini mana ile absorbe eder
- Cooldown: 120 saniye  
- Süre: 30 saniye
- Defensive ability

### ⏰ Zaman Manipülasyonu (S9)
- Kısa süre için zamanı %50 yavaşlatır
- Cooldown: 300 saniye
- Süre: 5 saniye
- Area effect

### 🌌 Gerçeklik Yırtığı (S10)
- Güçlü bir boyutsal saldırı (300% hasar)
- Cooldown: 600 saniye
- Ultimate ability
- Single target

## Gelişmiş Özellikler

### Meditasyon Sistemi
- 30 dakika kesintisiz meditasyon gerekir
- Hareket veya saldırı meditasyonu keser
- Progress bar ile takip
- Garantili yükseltme sağlar

### Cooldown Yönetimi
- Method-specific cooldown'lar
- Database'de persistent storage
- Automatic cleanup

### İstatistik Takibi
- Toplam deneme sayısı
- Başarılı yükseltme sayısı
- Başarı oranı hesaplama
- Detailed logging

## Performans

### Optimizasyonlar
- Database indexing
- Memory caching
- Lazy loading
- Batch operations

### Sınırlamalar
- Günlük maksimum 3 deneme
- Character level 90+ gereksinimi
- Kingdom level gereksinimleri
- Method-specific cooldown'lar

## Güvenlik

### Korumalar
- SQL injection koruması
- Input validation
- Rate limiting
- Packet verification

## Gelecek Geliştirmeler

### Planlanan Özellikler
- **Sage PvP Bonusları**: PvP'de ek etkiler
- **Guild Sage Buffs**: Guild için özel bonuslar
- **Sage Tournaments**: Competitive events
- **Advanced Combinations**: Skill kombinasyonları
- **Sage Masteries**: Specialization paths

### Genişletme Alanları
- Daha fazla özel yetenek
- Cross-skill synergies
- Sage-specific items
- Advanced meditation techniques

## Uyumluluk

### Sistem Gereksinimleri
- Metin2 Source 2013+
- MySQL 5.7+
- Python 2.7
- Kingdom System (opsiyonel)

### Entegrasyonlar
- ✅ Kingdom System ile uyumlu
- ✅ Offline Shop System ile uyumlu
- ✅ Mevcut skill sistem ile uyumlu
- ✅ Standard Metin2 client ile uyumlu

## Lisans

Bu proje GPL v3 lisansı altında dağıtılmaktadır.

## Destek

- GitHub Issues
- Community Forum
- Discord Support

---

**Version**: 1.0  
**Release Date**: 10 Ekim 2025  
**Compatibility**: Yumulk Kingdom System + Offline Shop System  
**Author**: Yumulk Development Team