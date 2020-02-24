MODULE Person2;

TYPE
  Person = RECORD
             Height: INTEGER;
             Age: INTEGER
           END;
  PersonPtr = POINTER TO Person;

PROCEDURE Set(p: PersonPtr);
BEGIN
  p^.Age := 18;
END Set;

END Person2.
