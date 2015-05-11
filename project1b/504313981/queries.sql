select concat(first,' ', last) as Name
from Actor, Movie, MovieActor
where Movie.title = 'Die Another Day' and
Movie.id = MovieActor.mid and
MovieActor.aid = Actor.id;

select count(*)
	from (
	select count(*)
	from MovieActor
	group by aid
	having count(mid)>1) as myGroup;


select count(*)
from(
	select distinct MovieDirector.did
	from MovieGenre, MovieDirector
	where MovieGenre.genre = 'Crime' and
	MovieGenre.mid = MovieDirector.mid ) as crimeDirector;


select AngMovies.mid, AngMovies.title, count(aid)
from(
	select  MovieDirector.mid, Movie.title
	from Director, MovieDirector, Movie
	where Director.first = 'Ang' and
	Director.last = 'Lee' and
	Director.id = MovieDirector.did and
	Movie.id = MovieDirector.mid) as AngMovies left join MovieActor
	on AngMovies.mid = MovieActor.mid
group by AngMovies.mid;


--find those actors, who have acted in the movies directed by whom produced
--the most movies on record, have passed away by now.

select distinct Actor.first, Actor.last
from (
	  select did
	  from MovieDirector
	  group by did
	  having count(mid) >= all(
					  			select count(mid)
					  			from MovieDirector dir
					  			group by did
					  			)
	) as productiveDir, MovieActor, Actor, MovieDirector
where productiveDir.did = MovieDirector.did and
MovieDirector.mid = MovieActor.mid and
MovieActor.aid = Actor.id and
Actor.dod is not null;