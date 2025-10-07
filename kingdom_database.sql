-- Kingdom System Database Tables
-- Bu dosya oyun veritabanına eklenmesi gereken tabloları içerir

-- Krallık bilgilerini tutan tablo
CREATE TABLE IF NOT EXISTS `kingdom` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(20) NOT NULL,
  `description` varchar(100) DEFAULT '',
  `color_r` tinyint(3) unsigned NOT NULL DEFAULT '255',
  `color_g` tinyint(3) unsigned NOT NULL DEFAULT '255',
  `color_b` tinyint(3) unsigned NOT NULL DEFAULT '255',
  `flag` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `create_time` int(10) unsigned NOT NULL DEFAULT '0',
  `king_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`),
  KEY `king_id` (`king_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- Krallık üyelerini tutan tablo
CREATE TABLE IF NOT EXISTS `kingdom_member` (
  `kingdom_id` int(10) unsigned NOT NULL,
  `player_id` int(10) unsigned NOT NULL,
  `player_name` varchar(24) NOT NULL,
  `rank` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `join_time` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`kingdom_id`, `player_id`),
  KEY `player_id` (`player_id`),
  KEY `player_name` (`player_name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- Krallık davetlerini tutan tablo (opsiyonel)
CREATE TABLE IF NOT EXISTS `kingdom_invite` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `kingdom_id` int(10) unsigned NOT NULL,
  `inviter_id` int(10) unsigned NOT NULL,
  `target_player_name` varchar(24) NOT NULL,
  `invite_time` int(10) unsigned NOT NULL DEFAULT '0',
  `status` tinyint(3) unsigned NOT NULL DEFAULT '0', -- 0: pending, 1: accepted, 2: declined, 3: expired
  PRIMARY KEY (`id`),
  KEY `kingdom_id` (`kingdom_id`),
  KEY `target_player_name` (`target_player_name`),
  KEY `invite_time` (`invite_time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- Krallık loglarını tutan tablo (opsiyonel)
CREATE TABLE IF NOT EXISTS `kingdom_log` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `kingdom_id` int(10) unsigned NOT NULL,
  `player_id` int(10) unsigned NOT NULL DEFAULT '0',
  `action` varchar(50) NOT NULL,
  `target_player_name` varchar(24) DEFAULT '',
  `message` varchar(255) DEFAULT '',
  `log_time` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `kingdom_id` (`kingdom_id`),
  KEY `log_time` (`log_time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- Player tablosuna krallık ID'si ekleme (mevcut player tablosu varsa)
-- ALTER TABLE player ADD COLUMN kingdom_id int(10) unsigned NOT NULL DEFAULT '0';
-- ALTER TABLE player ADD KEY kingdom_id (kingdom_id);

-- Örnek krallık verileri (test için)
INSERT INTO `kingdom` (`id`, `name`, `description`, `color_r`, `color_g`, `color_b`, `flag`, `create_time`, `king_id`) VALUES
(1, 'Ejder Krallığı', 'Güçlü savaşçıların krallığı', 255, 0, 0, 0, UNIX_TIMESTAMP(), 1),
(2, 'Mavi Deniz', 'Denizlerin efendileri', 0, 100, 255, 1, UNIX_TIMESTAMP(), 2),
(3, 'Altın Orman', 'Doğanın koruyucuları', 255, 215, 0, 2, UNIX_TIMESTAMP(), 3);

-- Örnek üye verileri (test için)
INSERT INTO `kingdom_member` (`kingdom_id`, `player_id`, `player_name`, `rank`, `join_time`) VALUES
(1, 1, 'KralEjder', 3, UNIX_TIMESTAMP()),
(1, 4, 'SavaşçıAli', 1, UNIX_TIMESTAMP()),
(1, 5, 'KomutanAyşe', 2, UNIX_TIMESTAMP()),
(2, 2, 'DenizKralı', 3, UNIX_TIMESTAMP()),
(2, 6, 'GemiciMehmet', 0, UNIX_TIMESTAMP()),
(3, 3, 'OrmanKralı', 3, UNIX_TIMESTAMP()),
(3, 7, 'AvcıFatma', 1, UNIX_TIMESTAMP());

-- Rütbe açıklamaları:
-- 0: Üye (Member)
-- 1: Subay (Officer) 
-- 2: Komutan (Commander)
-- 3: Kral (King)

-- Flag açıklamaları:
-- 0: Ejder bayrağı
-- 1: Deniz bayrağı
-- 2: Orman bayrağı
-- 3: Güneş bayrağı
-- 4: Ay bayrağı
