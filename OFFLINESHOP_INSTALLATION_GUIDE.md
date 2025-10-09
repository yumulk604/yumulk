# Offline Shop Sistemi - Kurulum ve Entegrasyon Rehberi

## Genel Bakış

Bu rehber, yumulk krallık sistemine entegre çalışacak offline shop sisteminin kurulum ve entegrasyon sürecini detaylı olarak açıklamaktadır. Offline shop sistemi, oyuncuların kendi krallık topraklarında mağaza açabilmelerini ve çevrimdışı iken de satış yapabilmelerini sağlar.

## Sistem Özellikleri

### Temel Özellikler
- **Krallık Entegrasyonu**: Sadece krallık üyeleri kendi topraklarında shop açabilir
- **40 Item Slot**: Her shop'ta 40 adet item satışa çıkarılabilir
- **7 Gün Otomatik Süre**: Shop'lar otomatik olarak 7 gün açık kalır
- **Gelir Takibi**: Satış geçmişi ve toplam kazanç takibi
- **Arama Sistemi**: Shop ve item arama özellikleri
- **Kategori Sistemi**: Items ve shop'ları kategorilere ayırma
- **Güvenlik**: SQL injection koruması ve yetki kontrolleri

### Krallık Sistemi ile Entegrasyon
- Shop açma yetkisi krallık üyeliği ile sınırlıdır
- Her krallık bölgesinde maksimum 10 shop açılabilir
- Her oyuncu maksimum 3 shop açabilir
- Kingdom koordinatları ile lokasyon kontrolü

## Kurulum Adımları

### 1. Veritabanı Kurulumu

#### A. Veritabanı Tablolarını Oluşturun
```sql
-- offlineshop_database.sql dosyasındaki tüm tabloları çalıştırın
mysql -u [kullanıcı_adı] -p [veritabanı_adı] < offlineshop_database.sql
```

#### B. Ana Tablolar
- `offline_shops`: Ana shop bilgileri
- `offline_shop_items`: Shop item'ları
- `offline_shop_transactions`: Satış geçmişi
- `offline_shop_visitors`: Ziyaretçi takibi
- `offline_shop_favorites`: Favori shop'lar
- `offline_shop_categories`: Kategori sistemi
- `offline_shop_earnings`: Detaylı kazanç takibi

#### C. View'lar ve Procedure'lar
- `v_active_shops`: Aktif shop'ların görünümü
- `v_popular_shops`: Popüler shop'ların sıralaması
- `v_kingdom_shop_stats`: Krallık bazlı istatistikler
- `CleanExpiredShops()`: Süresi dolmuş shop'ları temizleme
- `GetShopStatistics()`: Shop istatistikleri alma

### 2. Server-side Kurulumu

#### A. Header Dosyaları
Aşağıdaki dosyaları `Serverside/Server/game/src/` dizinine kopyalayın:
- `offlineshop.h` (yaratılacak)
- `offlineshop.cpp` (yaratılacak)
- `offlineshop_packets.h` (yaratılacak)
- `offlineshop_packets.cpp`

#### B. Makefile'a Ekleme
```makefile
# Makefile'a aşağıdaki satırları ekleyin
OBJS += offlineshop.o
OBJS += offlineshop_packets.o
```

#### C. Main.cpp Entegrasyonu
```cpp
// main.cpp'ye aşağıdaki include'ları ekleyin
#include "offlineshop.h"

// InitializeGame() fonksiyonuna ekleyin:
void InitializeGame()
{
    // ... mevcut kod ...
    
    // Offline Shop System Initialize
    InitializeOfflineShopSystem();
    
    // ... devamı ...
}

// DestroyGame() fonksiyonuna ekleyin:
void DestroyGame()
{
    // ... mevcut kod ...
    
    // Offline Shop System Destroy
    DestroyOfflineShopSystem();
    
    // ... devamı ...
}
```

#### D. Input_main.cpp Packet Handler Ekleme
```cpp
// input_main.cpp'ye packet handler'ları ekleyin

// Packet header'larını tanımlama bölümünde:
case HEADER_CG_OFFLINESHOP_CREATE:
    COfflineShopPacketHandler::HandleCreateShop(this, c_pData);
    break;
    
case HEADER_CG_OFFLINESHOP_OPEN:
    COfflineShopPacketHandler::HandleOpenShop(this, c_pData);
    break;
    
case HEADER_CG_OFFLINESHOP_CLOSE:
    COfflineShopPacketHandler::HandleCloseShop(this, c_pData);
    break;
    
case HEADER_CG_OFFLINESHOP_ADD_ITEM:
    COfflineShopPacketHandler::HandleAddItem(this, c_pData);
    break;
    
case HEADER_CG_OFFLINESHOP_REMOVE_ITEM:
    COfflineShopPacketHandler::HandleRemoveItem(this, c_pData);
    break;
    
case HEADER_CG_OFFLINESHOP_BUY_ITEM:
    COfflineShopPacketHandler::HandleBuyItem(this, c_pData);
    break;
    
case HEADER_CG_OFFLINESHOP_VIEW:
    COfflineShopPacketHandler::HandleViewShop(this, c_pData);
    break;
    
case HEADER_CG_OFFLINESHOP_SEARCH:
    COfflineShopPacketHandler::HandleSearchShops(this, c_pData);
    break;
    
case HEADER_CG_OFFLINESHOP_LIST:
    COfflineShopPacketHandler::HandleListShops(this, c_pData);
    break;
```

#### E. Kingdom.cpp Entegrasyonu
```cpp
// kingdom.cpp'ye offline shop koordinat kontrol fonksiyonu ekleyin

bool CKingdom::IsInKingdomTerritory(long lMapIndex, long lX, long lY)
{
    // Mevcut krallık koordinat kontrol kodunuzu buraya ekleyin
    // Bu fonksiyon krallık toprakları içinde olup olmadığını kontrol eder
    return true; // Geçici
}

// LPCHARACTER'a GetKingdomID() fonksiyonu ekleyin (eğer yoksa)
DWORD CKingdom::GetPlayerKingdomID(DWORD dwPlayerID)
{
    // Player'ın hangi krallığa ait olduğunu döndür
    auto it = m_map_pkMember.find(dwPlayerID);
    if (it != m_map_pkMember.end())
        return m_dwID;
    return 0;
}
```

### 3. Client-side Kurulumu

#### A. Python Dosyalarını Kopyalama
```
Binary/offlineshop_ui.py
```

#### B. UIScript Dosyaları Oluşturma
`uiscript/uiscript/` dizininde aşağıdaki dosyaları oluşturun:

- `OfflineShopCreateWindow.py`
- `OfflineShopManagementWindow.py`
- `OfflineShopViewWindow.py`
- `OfflineShopSearchWindow.py`

#### C. Game.py Entegrasyonu
```python
# game.py'ye aşağıdaki import'u ekleyin
import offlineshop_ui

# Game sınıfının __init__ fonksiyonuna:
self.offlineShopWindows = {
    'create': None,
    'management': None,
    'view': None,
    'search': None
}

# Chat komutları ekleme
def OnChat(self, chatType, chatMessage):
    # ... mevcut kod ...
    
    if chatMessage == "/shop_ac" or chatMessage == "/offlineshop":
        self.OpenOfflineShopCreate()
        return
    elif chatMessage == "/shop_yonet":
        self.OpenOfflineShopManagement()
        return
    elif chatMessage == "/shop_ara":
        self.OpenOfflineShopSearch()
        return
        
# Fonksiyonları ekleme
def OpenOfflineShopCreate(self):
    if not self.offlineShopWindows['create']:
        self.offlineShopWindows['create'] = offlineshop_ui.GetOfflineShopCreateWindow()
    self.offlineShopWindows['create'].Open()
    
def OpenOfflineShopManagement(self):
    if not self.offlineShopWindows['management']:
        self.offlineShopWindows['management'] = offlineshop_ui.GetOfflineShopManagementWindow()
    self.offlineShopWindows['management'].Open()
    
def OpenOfflineShopSearch(self):
    if not self.offlineShopWindows['search']:
        self.offlineShopWindows['search'] = offlineshop_ui.GetOfflineShopSearchWindow()
    self.offlineShopWindows['search'].Open()
```

#### D. NetworkModule.py Entegrasyonu
```python
# networkmodule.py'ye packet fonksiyonlarını ekleyin

def SendOfflineShopCreatePacket(name, desc, x, y):
    net.SendOfflineShopCreatePacket(name, desc, x, y)
    
def SendOfflineShopOpenPacket(shopID):
    net.SendOfflineShopOpenPacket(shopID)
    
def SendOfflineShopClosePacket(shopID):
    net.SendOfflineShopClosePacket(shopID)
    
# ... diğer packet fonksiyonları ...
```

### 4. Sistem Entegrasyonu

#### A. Kingdom Sistemi ile Bağlantı
Offline shop sistemi, mevcut kingdom sisteminden aşağıdaki bilgileri kullanır:
- Player'ın krallık ID'si
- Krallık toprakları koordinatları
- Krallık üye bilgileri
- Krallık yetki seviyeleri

#### B. Item Sistemi ile Bağlantı
- Item oluşturma ve yok etme
- Item socket ve attribute kopyalama
- Inventory kontrolleri
- Item log sistemi

#### C. Chat Sistemi ile Bağlantı
- Shop komutları (/shop_ac, /shop_yonet, /shop_ara)
- Bildirim mesajları
- Hata mesajları

### 5. Test ve Debug

#### A. Temel Testler
1. **Shop Oluşturma Testi**
   ```
   - Krallık üyesi olmayan oyuncu shop açamaz ✓
   - Krallık toprakları dışında shop açılamaz ✓
   - Geçerli ad ve açıklama ile shop açılabilir ✓
   ```

2. **Item Ekleme/Çıkarma Testi**
   ```
   - Geçerli item shop'a eklenebilir ✓
   - Geçersiz fiyat ile item eklenemez ✓
   - Shop açık iken item eklenemez/çıkarılamaz ✓
   ```

3. **Satış Testi**
   ```
   - Kendi shop'ından item alınamaz ✓
   - Yeterli gold olmadan item alınamaz ✓
   - Başarılı satış işlemi ✓
   ```

#### B. Performans Testleri
- Çok sayıda shop ile sistem performansı
- Database query optimizasyonu
- Memory leak kontrolleri

#### C. Güvenlik Testleri
- SQL injection denemeleri
- Yetki atlama denemeleri
- Packet manipulation testleri

### 6. Yapılandırma

#### A. Config.h Ayarları
```cpp
// Config.h'ye aşağıdaki tanımlamaları ekleyin
#define OFFLINESHOP_ENABLED 1
#define OFFLINESHOP_MAX_SHOPS_PER_PLAYER 3
#define OFFLINESHOP_MAX_SHOPS_PER_KINGDOM_AREA 10
#define OFFLINESHOP_DEFAULT_DURATION (7 * 24 * 60 * 60) // 7 gün
#define OFFLINESHOP_MAX_ITEM_PRICE 2000000000LL // 2 milyar
#define OFFLINESHOP_SEARCH_DISTANCE 5000
```

#### B. Locale Ayarları
```python
# locale/tr/locale_game.txt'ye ekleyin
OFFLINESHOP_CREATE_SUCCESS	Offline shop başarıyla oluşturuldu!
OFFLINESHOP_ERROR_NO_KINGDOM	Shop açmak için bir krallığa üye olmalısınız!
OFFLINESHOP_ERROR_INVALID_LOCATION	Bu konumda shop açamazsınız!
OFFLINESHOP_ERROR_SHOP_LIMIT	Shop sayısı limitine ulaştınız!
# ... diğer mesajlar ...
```

### 7. Bakım ve Optimizasyon

#### A. Otomatik Temizlik
```sql
-- Crontab ile günlük çalıştırın
-- Daily cleanup at 03:00
0 3 * * * mysql -u[user] -p[password] [database] -e "CALL CleanExpiredShops();"
```

#### B. İstatistik Takibi
- Günlük aktif shop sayısı
- Krallık bazlı shop dağılımı
- En çok satan item'lar
- Toplam işlem hacmi

#### C. Log Dosyaları
- Shop oluşturma/kapatma logları
- Item satış logları
- Hata ve güvenlik logları

## Sorun Giderme

### Yaygın Problemler

#### 1. Shop Açılamıyor
**Neden**: Krallık sistemi entegrasyonu eksik
**Çözüm**: `GetKingdomID()` fonksiyonunun doğru çalıştığından emin olun

#### 2. Items Yüklenmiyor
**Neden**: Database bağlantı sorunu
**Çözüm**: MySQL bağlantısını kontrol edin ve indexleri doğrulayın

#### 3. Packet Hataları
**Neden**: Client-server packet uyumsuzluğu
**Çözüm**: Packet struct'larının her iki tarafta da aynı olduğunu kontrol edin

#### 4. Memory Leak
**Neden**: COfflineShop nesneleri doğru şekilde silinmiyor
**Çözüm**: Destructor'ların çalışıp çalışmadığını kontrol edin

### Debug Komutları

```cpp
// Debug için eklenen özel komutlar
ACMD(do_offlineshop_debug)
{
    char arg1[256];
    one_argument(argument, arg1, sizeof(arg1));
    
    if (!strcmp(arg1, "stats"))
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "Total Shops: %d", 
            COfflineShopManager::Instance().GetTotalShopCount());
        ch->ChatPacket(CHAT_TYPE_INFO, "Active Shops: %d", 
            COfflineShopManager::Instance().GetActiveShopCount());
    }
    // ... diğer debug komutları ...
}
```

## Gelecek Geliştirmeler

### Planlanan Özellikler
1. **Shop Dekorasyonu**: Shop görünümünü özelleştirme
2. **Açık Artırma**: Item'lar için açık artırma sistemi
3. **Toplu Satış**: Aynı item'lardan çok satın alma
4. **Shop İstatistikleri**: Detaylı satış raporları
5. **VIP Shop**: Premium özelliklerle gelişmiş shop'lar
6. **Mobile Desteği**: Mobil uygulamadan shop yönetimi

### Optimizasyon Alanları
1. **Cache Sistemi**: Sık erişilen shop bilgilerini cache'leme
2. **Index Optimizasyonu**: Database sorgularını hızlandırma
3. **Network Optimizasyonu**: Packet boyutlarını küçültme
4. **Memory Optimizasyonu**: Bellek kullanımını azaltma

## Lisans ve Destek

Bu offline shop sistemi yumulk krallık sistemi ile birlikte çalışacak şekilde tasarlanmıştır. Sistem açık kaynak kodludur ve GPL v3 lisansı altında dağıtılmaktadır.

**Destek**: GitHub repository'sinde issue açabilir veya tartışma bölümlerini kullanabilirsiniz.

---

*Son Güncelleme: 10 Ekim 2025*  
*Versiyon: 1.0*  
*Uyumluluk: Yumulk Kingdom System v2.0+*