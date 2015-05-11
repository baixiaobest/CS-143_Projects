create table Movie(
	id int not null,
	title varchar(100) not null,
	year int,
	rating varchar(10),
	company varchar(50),
	primary key(id),
	unique(id, title)
)ENGINE=INNODB;

create table Actor(
	id int not null,
	last varchar(20) not null,
	first varchar(20) not null,
	sex varchar(6),
	dob DATE not null,
	dod DATE,
	primary key(id),
	unique(id, last, first)
)ENGINE=INNODB;

create table Director(
	id int not null,
	last varchar(20) not null,
	first varchar(20) not null,
	dob DATE,
	dod DATE,
	primary key(id),
	unique(id, last, first)
)ENGINE=INNODB;

create table MovieGenre(
	mid int not null,
	genre varchar(20) not null,
	foreign key (mid) references Movie(id)
		on delete cascade
		on update cascade
) ENGINE=INNODB;

create table MovieDirector(
	mid int not null,
	did int not null,
	foreign key (mid) references Movie(id)
		on delete cascade
		on update cascade,
	foreign key (did) references Director(id)
		on delete cascade
		on update cascade
)ENGINE=INNODB;

create table MovieActor(
	mid int not null,
	aid int not null,
	role varchar(50) not null,
	foreign key (mid) references Movie(id)
		on delete cascade
		on update cascade,
	foreign key (aid) references Actor(id)
		on delete cascade
		on update cascade
)ENGINE=INNODB;

create table Review(
	name varchar(20) not null,
	time TIMESTAMP not null,
	mid int not null,
	rating int not null,
	comment varchar(500),
	foreign key (mid) references Movie(id)
		on delete cascade
		on update cascade
)ENGINE=INNODB;

create table MaxPersonID(
	id int not null
)ENGINE=INNODB;

create table MaxMovieID(
	id int not null
)ENGINE=INNODB;





