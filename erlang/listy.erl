-module (listy).
-export ([head/1, tail/1]).


head([X|_]) -> X.


tail([_]) -> [];
tail([_|Y]) -> Y.

