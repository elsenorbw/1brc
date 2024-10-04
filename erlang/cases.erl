-module(cases).
-export([insert/2, beach/1]).


insert(X,[]) -> [X];
insert(X, Set) ->
  case lists:member(X, Set) of 
    true -> Set;
    false -> [X|Set]
  end.


beach(Temperature) ->
  case Temperature of
    {celsius, N} when N >= 20, N =< 45 -> 'favourable';
    {kelvin, N} when N >= 293, N =< 318 -> 'scientifically favourable';
    {fahrenheit, N} when N >= 68, N =< 113 -> 'favourable in the USA';
    _ -> 'no beach'
  end.




