#!/usr/bin/env ruby
puts """create table student (
  sno char(8),
  sname char(16) unique,
  sage int,
  sgender char(1),
  primary key(sno)
);
"""
records = []
10000.times do |i|
  records.push({
    sno: i * 2,
    sname: rand(36 ** 14).to_s(36),
    sage: rand(30),
    sgender: "MF"[rand(2)],
  })
end
records.shuffle!
records.each do |record|
  puts "insert into student values ('#{record[:sno]}', '#{record[:sname]}', #{record[:sage]}, '#{record[:sgender]}');"
end
puts "drop table student;"
