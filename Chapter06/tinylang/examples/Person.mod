MODULE Person;

TYPE
  Person = RECORD
             Height: INTEGER;
             Age: INTEGER
           END;

PROCEDURE Set(VAR p: Person);
BEGIN
  p.Age := 18;
END Set;

END Person.
