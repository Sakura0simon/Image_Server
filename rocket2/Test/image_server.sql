DROP DATABASE IF EXISTS image_server;
CREATE DATABASE IF NOT EXISTS image_server;
USE image_server;

CREATE TABLE IF NOT EXISTS table_image(
	id INT PRIMARY KEY auto_increment,
	name VARCHAR(32) comment "图片名称",
	fsize INT comment "图片大小",
	fpath VARCHAR(255) comment "文件存放路径名",
	furl VARCHAR(255) comment "前端访问图片的url",
	fmd5 VARCHAR(64) comment "文件的MD5值",
	ultime DATETIME comment "图片上传时间"
)ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1;

INSERT INTO table_image VALUES 
(null, "轻盈", 1024, "./image/11.jpg", "/image/11.jpg", "rerererererer", now()),
(null, "鹊起", 1025, "./image/22.jpg", "/image/22.jpg", "rerererererer", now()),
(null, "迎风", 1026, "./image/33.jpg", "/image/33.jpg", "rerererererer", now());