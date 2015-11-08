create table student (
  sno char(8),
  sname char(16) unique,
  sage int,
  sgender char(1),
  primary key(sno)
);
insert into student values ('15056', '1a9zww8wgiompb', 17, 'F');
insert into student values ('15406', 'jvarksaes6hlyg', 13, 'F');
insert into student values ('7028', '5of4tnj6ilj1qk', 26, 'F');
insert into student values ('11422', '7xgzza0j2l8hyd', 25, 'M');
insert into student values ('12214', 'ikvxyauy3dvwuk', 7, 'F');
insert into student values ('23423', 'ikvxyauy3dvwuk', 7, 'F');
select * from student;
select * from student where sno = '15056';
delete from student where sno = '15056';
select * from student;
drop table student;
