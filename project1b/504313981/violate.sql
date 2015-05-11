--It failes to Update beacause id=3 already exists.
update Movie
	set id = 3
	where id = 2;

--This insert fail because there is primary key 1 already in the database.
--Duplicate of id is not allowed
insert into Actor
	values(1, 'Huang', 'Baixiao', 'Male', 1995-01-01, null);

--Again, duplicate id for id field
--because id is specified as primary key constraint
insert into Director
	values(16, 'Huang', 'Baixiao', 1995-01-01, null);

--foreign key constraint fails, movie id of 1 does not exist in Movie
insert into MovieGenre
	values(1, 'Game');

--director id does not exit, foreign key constraint
insert into MovieDirector
	values(2, 1);

--Movie id does not exit, foreign key constraint
insert into MovieDirector
	values(10000, 16);

--Actor id is not found in Actor, foreign key constraint
insert into MovieActor
	values(2, 2, 'Police');

--Movie id does not exist, foreign key constraint
insert into MovieActor
	values(10000, 10,'Police');

--no such movie id exists, foreign key constraint
insert into Review
	values('Baixiao', 2015-01-01, 10000, 5, 'very good');