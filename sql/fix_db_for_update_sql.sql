use realmd;
alter table account add last_local_ip varchar(30) NOT NULL DEFAULT '0.0.0.0' after last_ip;
use world;
rename table trinity_string to hellground_string;