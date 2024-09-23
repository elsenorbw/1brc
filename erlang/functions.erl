-module(functions).

-export([head/1, second/1, tail/1, same/2, valid_time/1, old_enough/1, right_age/1, wrong_age/1]).


head([H|_]) -> H.
second([_,B|_]) -> B.

tail([_|T]) -> T.


same(X,X) -> true;
same(_,_) -> false.

valid_time({Date = {Y,M,D}, Time = {H,Min,S}}) ->
io:format("The Date tuple (~p) says today is: ~p/~p/~p,~n",[Date,Y,M,D]),
io:format("The time tuple (~p) indicates: ~p:~p:~p.~n", [Time,H,Min,S]);
valid_time(_) ->
io:format("Stop feeding me wrong data!~n").


old_enough(X) when X >= 21 -> true;
old_enough(_) -> false.

right_age(X) when X >= 21, X =< 76 -> true;
right_age(_) -> false.

wrong_age(X) when X < 21; X > 76 -> true;
wrong_age(_) -> false.



