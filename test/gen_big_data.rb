#!/usr/bin/env ruby
puts """create table orders (
  orderkey int,
  custkey int unique,
  orderstatus char(1),
  totalprice float,
  clerk char(15),
  comments char(79) unique,
  primary key(orderkey)
);
create index custkeyidx on orders(custkey);
create index commentsidx on orders(comments);
"""
records = []
200000.times do |i|
  records.push({
    orderkey: i * 2,
    custkey: i * 3,
    orderstatus: "AB"[rand(2)],
    totalprice: rand() * 100,
    clerk: rand(36 ** 12).to_s(36),
    comments: rand(36 ** 35).to_s(36),
  })
end
records.shuffle!
records.each do |record|
  puts "insert into orders values (#{record[:orderkey]}, #{record[:custkey]}, '#{record[:orderstatus]}', #{record[:totalprice]}, '#{record[:clerk]}', '#{record[:comments]}');"
end
