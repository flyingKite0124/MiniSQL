create table student (
  sno char(8),
  sname char(16) unique,
  sage int,
  sgender char(1),
  primary key(sno)
);
insert into student values('12345678', 'wy', 19, 'M');
insert into student values('88888888', 'sample', 22, 'F');
select * from student where sno = '88888888';
select * from student where sage < 20;
select * from student where sage > 20 and sgender = 'F';
delete from student where sno = '88888888';
select * from student;
delete from student;
select * from student;
drop table student;
quit;
