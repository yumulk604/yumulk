-- Offline Shop System Database Schema
-- Bu dosya yumulk krallık sistemine entegre çalışacak offline shop sistemi için veritabanı yapısını içerir

-- Ana offline shop tablosu
CREATE TABLE IF NOT EXISTS `offline_shops` (
  `shop_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `owner_pid` int(10) unsigned NOT NULL,
  `kingdom_id` int(10) unsigned NOT NULL DEFAULT '0',
  `shop_name` varchar(32) NOT NULL DEFAULT '',
  `shop_desc` varchar(128) NOT NULL DEFAULT '',
  `map_index` int(11) NOT NULL DEFAULT '0',
  `pos_x` int(11) NOT NULL DEFAULT '0',
  `pos_y` int(11) NOT NULL DEFAULT '0',
  `is_open` tinyint(1) NOT NULL DEFAULT '0',
  `expire_time` int(10) unsigned NOT NULL DEFAULT '0',
  `total_earned` bigint(20) unsigned NOT NULL DEFAULT '0',
  `total_sales` int(10) unsigned NOT NULL DEFAULT '0',
  `created_time` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `last_access` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`shop_id`),
  KEY `idx_owner_pid` (`owner_pid`),
  KEY `idx_kingdom_id` (`kingdom_id`),
  KEY `idx_map_index` (`map_index`),
  KEY `idx_expire_time` (`expire_time`),
  KEY `idx_shop_name` (`shop_name`),
  KEY `idx_location` (`map_index`, `pos_x`, `pos_y`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Offline shop items tablosu
CREATE TABLE IF NOT EXISTS `offline_shop_items` (
  `item_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `shop_id` int(10) unsigned NOT NULL,
  `slot` tinyint(3) unsigned NOT NULL,
  `vnum` int(10) unsigned NOT NULL,
  `count` int(10) unsigned NOT NULL DEFAULT '1',
  `price` bigint(20) unsigned NOT NULL,
  `socket0` bigint(20) NOT NULL DEFAULT '0',
  `socket1` bigint(20) NOT NULL DEFAULT '0',
  `socket2` bigint(20) NOT NULL DEFAULT '0',
  `socket3` bigint(20) NOT NULL DEFAULT '0',
  `socket4` bigint(20) NOT NULL DEFAULT '0',
  `socket5` bigint(20) NOT NULL DEFAULT '0',
  `attr1_type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `attr1_value` int(11) NOT NULL DEFAULT '0',
  `attr2_type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `attr2_value` int(11) NOT NULL DEFAULT '0',
  `attr3_type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `attr3_value` int(11) NOT NULL DEFAULT '0',
  `attr4_type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `attr4_value` int(11) NOT NULL DEFAULT '0',
  `attr5_type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `attr5_value` int(11) NOT NULL DEFAULT '0',
  `attr6_type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `attr6_value` int(11) NOT NULL DEFAULT '0',
  `attr7_type` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `attr7_value` int(11) NOT NULL DEFAULT '0',
  `added_time` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`item_id`),
  UNIQUE KEY `idx_shop_slot` (`shop_id`, `slot`),
  KEY `idx_shop_id` (`shop_id`),
  KEY `idx_vnum` (`vnum`),
  KEY `idx_price` (`price`),
  FOREIGN KEY (`shop_id`) REFERENCES `offline_shops` (`shop_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Shop işlem geçmişi tablosu
CREATE TABLE IF NOT EXISTS `offline_shop_transactions` (
  `transaction_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `shop_id` int(10) unsigned NOT NULL,
  `buyer_pid` int(10) unsigned NOT NULL,
  `item_vnum` int(10) unsigned NOT NULL,
  `item_count` int(10) unsigned NOT NULL,
  `price` bigint(20) unsigned NOT NULL,
  `transaction_time` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `buyer_name` varchar(24) NOT NULL DEFAULT '',
  PRIMARY KEY (`transaction_id`),
  KEY `idx_shop_id` (`shop_id`),
  KEY `idx_buyer_pid` (`buyer_pid`),
  KEY `idx_transaction_time` (`transaction_time`),
  FOREIGN KEY (`shop_id`) REFERENCES `offline_shops` (`shop_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Shop ziyaretçi tablosu
CREATE TABLE IF NOT EXISTS `offline_shop_visitors` (
  `visit_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `shop_id` int(10) unsigned NOT NULL,
  `visitor_pid` int(10) unsigned NOT NULL,
  `visit_time` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `visitor_name` varchar(24) NOT NULL DEFAULT '',
  PRIMARY KEY (`visit_id`),
  KEY `idx_shop_id` (`shop_id`),
  KEY `idx_visitor_pid` (`visitor_pid`),
  KEY `idx_visit_time` (`visit_time`),
  FOREIGN KEY (`shop_id`) REFERENCES `offline_shops` (`shop_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Shop favoriler tablosu (oyuncular favori shopları takip edebilir)
CREATE TABLE IF NOT EXISTS `offline_shop_favorites` (
  `favorite_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `player_pid` int(10) unsigned NOT NULL,
  `shop_id` int(10) unsigned NOT NULL,
  `added_time` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`favorite_id`),
  UNIQUE KEY `idx_player_shop` (`player_pid`, `shop_id`),
  KEY `idx_player_pid` (`player_pid`),
  KEY `idx_shop_id` (`shop_id`),
  FOREIGN KEY (`shop_id`) REFERENCES `offline_shops` (`shop_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Shop kategorileri tablosu (gelecekteki geliştirmeler için)
CREATE TABLE IF NOT EXISTS `offline_shop_categories` (
  `category_id` tinyint(3) unsigned NOT NULL AUTO_INCREMENT,
  `category_name` varchar(32) NOT NULL,
  `category_desc` varchar(128) NOT NULL DEFAULT '',
  `is_active` tinyint(1) NOT NULL DEFAULT '1',
  PRIMARY KEY (`category_id`),
  UNIQUE KEY `idx_category_name` (`category_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Shop ve kategori ilişki tablosu
CREATE TABLE IF NOT EXISTS `offline_shop_category_relations` (
  `relation_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `shop_id` int(10) unsigned NOT NULL,
  `category_id` tinyint(3) unsigned NOT NULL,
  PRIMARY KEY (`relation_id`),
  UNIQUE KEY `idx_shop_category` (`shop_id`, `category_id`),
  KEY `idx_shop_id` (`shop_id`),
  KEY `idx_category_id` (`category_id`),
  FOREIGN KEY (`shop_id`) REFERENCES `offline_shops` (`shop_id`) ON DELETE CASCADE,
  FOREIGN KEY (`category_id`) REFERENCES `offline_shop_categories` (`category_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Shop kazanç geçmişi tablosu (detaylı finansal takip için)
CREATE TABLE IF NOT EXISTS `offline_shop_earnings` (
  `earning_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `shop_id` int(10) unsigned NOT NULL,
  `owner_pid` int(10) unsigned NOT NULL,
  `amount` bigint(20) unsigned NOT NULL,
  `earning_type` enum('SALE', 'WITHDRAW', 'FEE', 'BONUS') NOT NULL DEFAULT 'SALE',
  `description` varchar(255) NOT NULL DEFAULT '',
  `created_time` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`earning_id`),
  KEY `idx_shop_id` (`shop_id`),
  KEY `idx_owner_pid` (`owner_pid`),
  KEY `idx_earning_type` (`earning_type`),
  KEY `idx_created_time` (`created_time`),
  FOREIGN KEY (`shop_id`) REFERENCES `offline_shops` (`shop_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Örnek kategoriler ekleme
INSERT INTO `offline_shop_categories` (`category_name`, `category_desc`) VALUES
('Silahlar', 'Kılıç, Bıçak, Yay ve diğer silahlar'),
('Zırhlar', 'Vücut zırhları, kasklar, eldiven ve ayakkabılar'),
('Aksesuarlar', 'Kolye, küpe, yüzük ve diğer aksesuarlar'),
('Tüketim Eşyaları', 'İksir, yemek ve diğer tüketim eşyaları'),
('Taşlar', 'Ruh taşları, yükseltme taşları ve diğer taşlar'),
('Kitaplar', 'Beceri kitapları ve diğer kitaplar'),
('Çeşitli', 'Diğer çeşitli eşyalar');

-- Shop sistemi için özel view'lar
CREATE VIEW `v_active_shops` AS
SELECT 
    s.*,
    p.name as owner_name,
    k.name as kingdom_name,
    COUNT(si.item_id) as item_count,
    COUNT(st.transaction_id) as total_transactions
FROM offline_shops s
LEFT JOIN player p ON s.owner_pid = p.id
LEFT JOIN kingdom k ON s.kingdom_id = k.id
LEFT JOIN offline_shop_items si ON s.shop_id = si.shop_id
LEFT JOIN offline_shop_transactions st ON s.shop_id = st.shop_id
WHERE s.expire_time > UNIX_TIMESTAMP() AND s.is_open = 1
GROUP BY s.shop_id;

-- Popüler shoplar view'ı
CREATE VIEW `v_popular_shops` AS
SELECT 
    s.*,
    p.name as owner_name,
    COUNT(DISTINCT sv.visitor_pid) as unique_visitors,
    COUNT(st.transaction_id) as total_sales,
    COALESCE(SUM(st.price), 0) as total_revenue
FROM offline_shops s
LEFT JOIN player p ON s.owner_pid = p.id
LEFT JOIN offline_shop_visitors sv ON s.shop_id = sv.shop_id
LEFT JOIN offline_shop_transactions st ON s.shop_id = st.shop_id
WHERE s.expire_time > UNIX_TIMESTAMP() AND s.is_open = 1
GROUP BY s.shop_id
ORDER BY unique_visitors DESC, total_sales DESC;

-- Kingdom shop istatistikleri view'ı
CREATE VIEW `v_kingdom_shop_stats` AS
SELECT 
    k.id as kingdom_id,
    k.name as kingdom_name,
    COUNT(s.shop_id) as total_shops,
    COUNT(CASE WHEN s.is_open = 1 AND s.expire_time > UNIX_TIMESTAMP() THEN 1 END) as active_shops,
    COALESCE(SUM(s.total_earned), 0) as total_kingdom_earnings,
    COALESCE(AVG(s.total_earned), 0) as avg_shop_earnings
FROM kingdom k
LEFT JOIN offline_shops s ON k.id = s.kingdom_id
GROUP BY k.id, k.name;

-- Temizlik için stored procedure'lar
DELIMITER //

CREATE PROCEDURE CleanExpiredShops()
BEGIN
    DECLARE done INT DEFAULT FALSE;
    DECLARE shop_id_to_delete INT;
    DECLARE cur CURSOR FOR 
        SELECT shop_id FROM offline_shops 
        WHERE expire_time < UNIX_TIMESTAMP() - (7 * 24 * 60 * 60); -- 7 gün sonra tamamen sil
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = TRUE;
    
    OPEN cur;
    read_loop: LOOP
        FETCH cur INTO shop_id_to_delete;
        IF done THEN
            LEAVE read_loop;
        END IF;
        
        -- Shop ve ilgili tüm verileri sil
        DELETE FROM offline_shops WHERE shop_id = shop_id_to_delete;
    END LOOP;
    CLOSE cur;
END //

CREATE PROCEDURE GetShopStatistics(IN shop_id_param INT)
BEGIN
    SELECT 
        s.*,
        p.name as owner_name,
        k.name as kingdom_name,
        COUNT(DISTINCT si.item_id) as total_items,
        COUNT(DISTINCT st.transaction_id) as total_transactions,
        COUNT(DISTINCT sv.visitor_pid) as unique_visitors,
        COALESCE(MAX(st.transaction_time), s.created_time) as last_sale_time
    FROM offline_shops s
    LEFT JOIN player p ON s.owner_pid = p.id
    LEFT JOIN kingdom k ON s.kingdom_id = k.id
    LEFT JOIN offline_shop_items si ON s.shop_id = si.shop_id
    LEFT JOIN offline_shop_transactions st ON s.shop_id = st.shop_id
    LEFT JOIN offline_shop_visitors sv ON s.shop_id = sv.shop_id
    WHERE s.shop_id = shop_id_param
    GROUP BY s.shop_id;
END //

DELIMITER ;

-- Otomatik temizlik için event scheduler (opsiyonel)
-- SET GLOBAL event_scheduler = ON;
-- CREATE EVENT IF NOT EXISTS cleanup_expired_shops
-- ON SCHEDULE EVERY 1 DAY
-- STARTS CURRENT_TIMESTAMP
-- DO CALL CleanExpiredShops();

-- İndeksler ve optimizasyonlar
ALTER TABLE offline_shops ADD INDEX idx_active_shops (is_open, expire_time);
ALTER TABLE offline_shop_items ADD INDEX idx_item_search (vnum, price);
ALTER TABLE offline_shop_transactions ADD INDEX idx_daily_stats (transaction_time, shop_id);

-- Trigger'lar
DELIMITER //

CREATE TRIGGER update_shop_stats_after_sale
AFTER INSERT ON offline_shop_transactions
FOR EACH ROW
BEGIN
    UPDATE offline_shops 
    SET total_earned = total_earned + NEW.price,
        total_sales = total_sales + 1,
        last_access = NOW()
    WHERE shop_id = NEW.shop_id;
END //

CREATE TRIGGER log_shop_visit
AFTER INSERT ON offline_shop_visitors
FOR EACH ROW
BEGIN
    UPDATE offline_shops 
    SET last_access = NOW()
    WHERE shop_id = NEW.shop_id;
END //

DELIMITER ;

COMMIT;