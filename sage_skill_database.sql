-- Sage Skill System Database Schema
-- Perfect seviyesinden sonra gelen Sage skill sistemi için veritabanı yapısı
-- Normal -> Master -> Grand Master -> Perfect -> SAGE (S1-S10)

-- Ana sage skills tablosu
CREATE TABLE IF NOT EXISTS `sage_skills` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `player_id` int(10) unsigned NOT NULL,
  `skill_vnum` int(10) unsigned NOT NULL,
  `sage_grade` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `last_upgrade_time` int(10) unsigned NOT NULL DEFAULT '0',
  `upgrade_attempts` int(10) unsigned NOT NULL DEFAULT '0',
  `successful_upgrades` int(10) unsigned NOT NULL DEFAULT '0',
  `upgrade_method` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `special_abilities` int(10) unsigned NOT NULL DEFAULT '0',
  `total_damage_bonus` float NOT NULL DEFAULT '0',
  `total_cooldown_reduction` float NOT NULL DEFAULT '0',
  `total_mana_reduction` float NOT NULL DEFAULT '0',
  `created_time` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `updated_time` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx_player_skill` (`player_id`, `skill_vnum`),
  KEY `idx_player_id` (`player_id`),
  KEY `idx_skill_vnum` (`skill_vnum`),
  KEY `idx_sage_grade` (`sage_grade`),
  KEY `idx_upgrade_time` (`last_upgrade_time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Sage skill yükseltme geçmişi tablosu
CREATE TABLE IF NOT EXISTS `sage_skill_upgrade_log` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `player_id` int(10) unsigned NOT NULL,
  `skill_vnum` int(10) unsigned NOT NULL,
  `old_grade` tinyint(3) unsigned NOT NULL,
  `new_grade` tinyint(3) unsigned NOT NULL,
  `upgrade_method` tinyint(3) unsigned NOT NULL,
  `success` tinyint(1) NOT NULL DEFAULT '0',
  `success_rate` float NOT NULL DEFAULT '0',
  `items_consumed` text,
  `upgrade_time` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `player_name` varchar(24) NOT NULL DEFAULT '',
  `player_level` int(10) unsigned NOT NULL DEFAULT '0',
  `kingdom_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `idx_player_id` (`player_id`),
  KEY `idx_skill_vnum` (`skill_vnum`),
  KEY `idx_upgrade_time` (`upgrade_time`),
  KEY `idx_success` (`success`),
  KEY `idx_upgrade_method` (`upgrade_method`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Sage skill cooldown tablosu
CREATE TABLE IF NOT EXISTS `sage_skill_cooldowns` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `player_id` int(10) unsigned NOT NULL,
  `cooldown_type` tinyint(3) unsigned NOT NULL,
  `expire_time` int(10) unsigned NOT NULL,
  `skill_vnum` int(10) unsigned NOT NULL DEFAULT '0',
  `created_time` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx_player_cooldown` (`player_id`, `cooldown_type`),
  KEY `idx_expire_time` (`expire_time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Sage özel yetenekler tablosu
CREATE TABLE IF NOT EXISTS `sage_special_abilities` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `player_id` int(10) unsigned NOT NULL,
  `skill_vnum` int(10) unsigned NOT NULL,
  `ability_id` int(10) unsigned NOT NULL,
  `ability_level` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `unlock_time` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `usage_count` int(10) unsigned NOT NULL DEFAULT '0',
  `last_used` datetime DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx_player_skill_ability` (`player_id`, `skill_vnum`, `ability_id`),
  KEY `idx_player_id` (`player_id`),
  KEY `idx_ability_id` (`ability_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Sage meditasyon oturumları tablosu
CREATE TABLE IF NOT EXISTS `sage_meditation_sessions` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `player_id` int(10) unsigned NOT NULL,
  `skill_vnum` int(10) unsigned NOT NULL,
  `start_time` int(10) unsigned NOT NULL,
  `duration` int(10) unsigned NOT NULL DEFAULT '1800', -- 30 dakika
  `completed` tinyint(1) NOT NULL DEFAULT '0',
  `interrupted` tinyint(1) NOT NULL DEFAULT '0',
  `meditation_type` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `bonus_multiplier` float NOT NULL DEFAULT '1.0',
  `created_time` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `idx_player_id` (`player_id`),
  KEY `idx_start_time` (`start_time`),
  KEY `idx_completed` (`completed`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Sage item proto eklentileri (varolan item_proto'ya eklenecek)
-- Bu itemler sistem tarafından otomatik olarak tanınmalı

-- Sage skill bonusları konfigurasyonu
CREATE TABLE IF NOT EXISTS `sage_skill_config` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `sage_grade` tinyint(3) unsigned NOT NULL,
  `damage_multiplier` float NOT NULL DEFAULT '1.0',
  `cooldown_reduction` float NOT NULL DEFAULT '0.0',
  `mana_reduction` float NOT NULL DEFAULT '0.0',
  `special_effect_id` int(10) unsigned NOT NULL DEFAULT '0',
  `unlock_new_ability` tinyint(1) NOT NULL DEFAULT '0',
  `kingdom_requirement` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `min_character_level` int(10) unsigned NOT NULL DEFAULT '90',
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx_sage_grade` (`sage_grade`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Sage skill bonus konfigürasyon verilerini ekle
INSERT INTO `sage_skill_config` (`sage_grade`, `damage_multiplier`, `cooldown_reduction`, `mana_reduction`, `special_effect_id`, `unlock_new_ability`, `kingdom_requirement`, `min_character_level`) VALUES
(1, 1.05, 0.02, 0.015, 0, 0, 5, 90),
(2, 1.10, 0.04, 0.030, 0, 0, 5, 92),
(3, 1.15, 0.06, 0.045, 1, 1, 6, 94), -- Chain Lightning
(4, 1.20, 0.08, 0.060, 0, 0, 6, 96),
(5, 1.25, 0.10, 0.075, 2, 1, 7, 98), -- Double Cast
(6, 1.30, 0.12, 0.090, 0, 0, 7, 100),
(7, 1.35, 0.14, 0.105, 3, 1, 8, 102), -- Mana Shield
(8, 1.40, 0.16, 0.120, 0, 0, 8, 104),
(9, 1.45, 0.18, 0.135, 4, 1, 9, 106), -- Time Manipulation
(10, 1.50, 0.20, 0.150, 5, 1, 10, 108); -- Reality Tear

-- Sage yükseltme başarı oranları tablosu
CREATE TABLE IF NOT EXISTS `sage_upgrade_rates` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `current_grade` tinyint(3) unsigned NOT NULL,
  `upgrade_method` tinyint(3) unsigned NOT NULL,
  `base_success_rate` float NOT NULL,
  `level_bonus_per_level` float NOT NULL DEFAULT '0.001',
  `kingdom_bonus_per_level` float NOT NULL DEFAULT '0.01',
  `min_success_rate` float NOT NULL DEFAULT '0.05',
  `max_success_rate` float NOT NULL DEFAULT '0.95',
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx_grade_method` (`current_grade`, `upgrade_method`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Sage yükseltme başarı oranlarını ekle
-- Ancient Scroll (method 1)
INSERT INTO `sage_upgrade_rates` (`current_grade`, `upgrade_method`, `base_success_rate`) VALUES
(0, 1, 0.30), (1, 1, 0.28), (2, 1, 0.26), (3, 1, 0.24), (4, 1, 0.22),
(5, 1, 0.20), (6, 1, 0.18), (7, 1, 0.16), (8, 1, 0.14), (9, 1, 0.12);

-- Sage Stone (method 2)
INSERT INTO `sage_upgrade_rates` (`current_grade`, `upgrade_method`, `base_success_rate`) VALUES
(0, 2, 0.50), (1, 2, 0.47), (2, 2, 0.44), (3, 2, 0.41), (4, 2, 0.38),
(5, 2, 0.35), (6, 2, 0.32), (7, 2, 0.29), (8, 2, 0.26), (9, 2, 0.23);

-- Meditation (method 3) - Garantili ama uzun sürer
INSERT INTO `sage_upgrade_rates` (`current_grade`, `upgrade_method`, `base_success_rate`) VALUES
(0, 3, 1.00), (1, 3, 1.00), (2, 3, 1.00), (3, 3, 1.00), (4, 3, 1.00),
(5, 3, 1.00), (6, 3, 1.00), (7, 3, 1.00), (8, 3, 1.00), (9, 3, 1.00);

-- Özel yetenekler listesi
CREATE TABLE IF NOT EXISTS `sage_abilities_proto` (
  `ability_id` int(10) unsigned NOT NULL,
  `ability_name` varchar(64) NOT NULL,
  `ability_desc` text,
  `required_grade` tinyint(3) unsigned NOT NULL,
  `cooldown` int(10) unsigned NOT NULL DEFAULT '0',
  `mana_cost` int(10) unsigned NOT NULL DEFAULT '0',
  `effect_duration` int(10) unsigned NOT NULL DEFAULT '0',
  `effect_value` float NOT NULL DEFAULT '0',
  `target_type` tinyint(3) unsigned NOT NULL DEFAULT '0', -- 0=self, 1=target, 2=area
  `animation_id` int(10) unsigned NOT NULL DEFAULT '0',
  `sound_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ability_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Özel yetenekleri ekle
INSERT INTO `sage_abilities_proto` (`ability_id`, `ability_name`, `ability_desc`, `required_grade`, `cooldown`, `mana_cost`, `effect_duration`, `effect_value`, `target_type`) VALUES
(1, 'Chain Lightning', 'Zincir şimşek - hedefi ve yakındaki düşmanlara yıldırım hasارı verir', 3, 30, 100, 0, 1.5, 2),
(2, 'Double Cast', 'Çift Büyü - sonraki büyünüzü iki kez kullanır', 5, 60, 150, 10, 2.0, 0),
(3, 'Mana Shield', 'Mana Kalkanı - gelen hasarın bir kısmını mana ile absorbe eder', 7, 120, 200, 30, 0.5, 0),
(4, 'Time Manipulation', 'Zaman Manipülasyonu - kısa süre için zamanı yavaşlatır', 9, 300, 300, 5, 0.5, 2),
(5, 'Reality Tear', 'Gerçeklik Yırtığı - güçlü bir boyutsal saldırı yapar', 10, 600, 500, 0, 3.0, 1);

-- Sage istatistikleri view'u
CREATE VIEW `v_sage_skill_stats` AS
SELECT 
    ss.player_id,
    p.name as player_name,
    ss.skill_vnum,
    ss.sage_grade,
    ss.upgrade_attempts,
    ss.successful_upgrades,
    ROUND((ss.successful_upgrades * 100.0 / GREATEST(ss.upgrade_attempts, 1)), 2) as success_rate,
    ss.total_damage_bonus,
    ss.total_cooldown_reduction,
    ss.total_mana_reduction,
    ss.special_abilities,
    ss.last_upgrade_time,
    ss.created_time
FROM sage_skills ss
LEFT JOIN player p ON ss.player_id = p.id;

-- Popüler sage skilleri view'u
CREATE VIEW `v_popular_sage_skills` AS
SELECT 
    skill_vnum,
    COUNT(*) as player_count,
    AVG(sage_grade) as avg_grade,
    MAX(sage_grade) as max_grade,
    SUM(upgrade_attempts) as total_attempts,
    SUM(successful_upgrades) as total_successes,
    ROUND((SUM(successful_upgrades) * 100.0 / GREATEST(SUM(upgrade_attempts), 1)), 2) as overall_success_rate
FROM sage_skills
WHERE sage_grade > 0
GROUP BY skill_vnum
ORDER BY player_count DESC, avg_grade DESC;

-- Kingdom sage istatistikleri view'u
CREATE VIEW `v_kingdom_sage_stats` AS
SELECT 
    k.id as kingdom_id,
    k.name as kingdom_name,
    COUNT(DISTINCT ss.player_id) as sage_players,
    COUNT(ss.id) as total_sage_skills,
    AVG(ss.sage_grade) as avg_sage_grade,
    MAX(ss.sage_grade) as max_sage_grade,
    SUM(ss.upgrade_attempts) as total_attempts,
    SUM(ss.successful_upgrades) as total_successes
FROM kingdom k
LEFT JOIN kingdom_member km ON k.id = km.kingdom_id
LEFT JOIN sage_skills ss ON km.player_id = ss.player_id
WHERE ss.sage_grade > 0
GROUP BY k.id, k.name;

-- Temizlik için stored procedure'lar
DELIMITER //

CREATE PROCEDURE CleanExpiredSageCooldowns()
BEGIN
    DELETE FROM sage_skill_cooldowns 
    WHERE expire_time < UNIX_TIMESTAMP();
END //

CREATE PROCEDURE GetSageSkillRanking(IN skill_vnum_param INT)
BEGIN
    SELECT 
        ROW_NUMBER() OVER (ORDER BY sage_grade DESC, successful_upgrades DESC) as rank_position,
        player_id,
        (SELECT name FROM player WHERE id = ss.player_id) as player_name,
        sage_grade,
        successful_upgrades,
        upgrade_attempts,
        ROUND((successful_upgrades * 100.0 / GREATEST(upgrade_attempts, 1)), 2) as success_rate,
        last_upgrade_time
    FROM sage_skills ss
    WHERE skill_vnum = skill_vnum_param AND sage_grade > 0
    ORDER BY sage_grade DESC, successful_upgrades DESC
    LIMIT 100;
END //

CREATE PROCEDURE GetPlayerSageProgress(IN player_id_param INT)
BEGIN
    SELECT 
        ss.*,
        (SELECT name FROM player WHERE id = ss.player_id) as player_name,
        ssc.damage_multiplier,
        ssc.cooldown_reduction,
        ssc.mana_reduction,
        ssc.special_effect_id,
        GROUP_CONCAT(sap.ability_name) as unlocked_abilities
    FROM sage_skills ss
    LEFT JOIN sage_skill_config ssc ON ss.sage_grade = ssc.sage_grade
    LEFT JOIN sage_special_abilities ssa ON ss.player_id = ssa.player_id AND ss.skill_vnum = ssa.skill_vnum
    LEFT JOIN sage_abilities_proto sap ON ssa.ability_id = sap.ability_id
    WHERE ss.player_id = player_id_param
    GROUP BY ss.id;
END //

DELIMITER ;

-- İndeksler ve optimizasyonlar
ALTER TABLE sage_skills ADD INDEX idx_player_grade (player_id, sage_grade);
ALTER TABLE sage_skill_upgrade_log ADD INDEX idx_daily_upgrades (DATE(upgrade_time), player_id);
ALTER TABLE sage_special_abilities ADD INDEX idx_ability_usage (ability_id, usage_count);
ALTER TABLE sage_meditation_sessions ADD INDEX idx_active_sessions (player_id, completed, interrupted);

-- Trigger'lar
DELIMITER //

CREATE TRIGGER update_sage_skill_bonuses
AFTER UPDATE ON sage_skills
FOR EACH ROW
BEGIN
    IF NEW.sage_grade != OLD.sage_grade THEN
        UPDATE sage_skills ss
        JOIN sage_skill_config ssc ON ss.sage_grade = ssc.sage_grade
        SET 
            ss.total_damage_bonus = ssc.damage_multiplier,
            ss.total_cooldown_reduction = ssc.cooldown_reduction,
            ss.total_mana_reduction = ssc.mana_reduction,
            ss.updated_time = NOW()
        WHERE ss.id = NEW.id;
    END IF;
END //

CREATE TRIGGER log_sage_upgrade_attempt
AFTER INSERT ON sage_skill_upgrade_log
FOR EACH ROW
BEGIN
    -- Günlük deneme sayısını kontrol et (maksimum 3)
    DECLARE daily_attempts INT DEFAULT 0;
    
    SELECT COUNT(*) INTO daily_attempts
    FROM sage_skill_upgrade_log
    WHERE player_id = NEW.player_id 
    AND DATE(upgrade_time) = DATE(NEW.upgrade_time);
    
    IF daily_attempts > 3 THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Daily sage upgrade limit exceeded';
    END IF;
END //

DELIMITER ;

-- Otomatik temizlik için event scheduler
CREATE EVENT IF NOT EXISTS cleanup_sage_cooldowns
ON SCHEDULE EVERY 1 HOUR
STARTS CURRENT_TIMESTAMP
DO CALL CleanExpiredSageCooldowns();

-- Günlük istatistik güncellemesi
CREATE EVENT IF NOT EXISTS daily_sage_stats_update
ON SCHEDULE EVERY 1 DAY
STARTS (CURRENT_TIMESTAMP + INTERVAL 1 DAY - INTERVAL HOUR(CURRENT_TIMESTAMP) HOUR - INTERVAL MINUTE(CURRENT_TIMESTAMP) MINUTE)
DO
BEGIN
    -- Günlük sage skill istatistiklerini güncelle
    INSERT INTO sage_daily_stats (stat_date, total_players, total_skills, total_attempts, total_successes)
    SELECT 
        CURDATE() - INTERVAL 1 DAY,
        COUNT(DISTINCT player_id),
        COUNT(*),
        SUM(upgrade_attempts),
        SUM(successful_upgrades)
    FROM sage_skills
    WHERE DATE(updated_time) = CURDATE() - INTERVAL 1 DAY;
END;

COMMIT;