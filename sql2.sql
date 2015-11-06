create table orders (
	orderkey		int,
	custkey			int unique,
	orderstatus		char(1),
	totalprice		float,
	clerk			char(15),
	comments		char(79)	 unique,
	primary key 		(orderkey)
 );
insert into orders values (541959,408677,'F',241827.84,'Clerk#000002574','test: check unique');
insert into orders values (541911,408671,'F',241827.84,'Clerk#000002574','test: check u111');
create index custidx on orders (custkey);
create index comidx on orders (comments);
