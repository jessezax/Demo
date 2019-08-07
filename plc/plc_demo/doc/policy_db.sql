DROP TABLE IF EXISTS `plc_table`;
CREATE TABLE `plc_table` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `user_id` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT '规则ID，1字节',
  `rule_id` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '规则ID，4字节',
  `msg_data`  varchar(4096) NOT NULL COMMENT '策略数据,进行base64编码后存储',
 PRIMARY KEY (`id`),
  UNIQUE KEY `unique_key` (`rule_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='策略表';