TCP server summing numbers in C
================================
## ENG An extract from the protocol details:
* Communication between client and server is based on a connection-oriented TCP protocol.
* Client sends one or more lines containing numbers. For every line ended with carriage return and line feed ("\r\n"), server returns a line with a single number - the sum of added values or an error message ("ERROR\r\n").
* Line can only contain digits and spaces (number terminators). Numbers must be separated with one space. Line can't be empty, can't start or end with a space.

## Implementation details:
* Server is listening on port 2019.
* There can be only one client at once.
* Max line size is 2048 bytes.
* Max value of a number is 2,147,483,647.

---

## PL Specyfikacja protokołu:
* Komunikacja pomiędzy klientem, a serwerem odbywa się przy pomocy połączenia strumieniowego. Klient wysyła jedną lub więcej linii zawierających liczby. Dla każdej odebranej linii serwer zwraca linię zawierającą pojedynczą liczbę (obliczoną sumę) bądź komunikat o błędzie.
* Ogólna definicja linii jest taka jak w wielu innych protokołach: ciąg drukowalnych znaków ASCII (być może pusty) zakończony dwuznakiem \r\n.
* Linia z liczbami zawierać może tylko cyfry i spacje. Ciągi cyfr należy interpretować jako liczby dziesiętne. Spacje służą jako separatory liczb; każda spacja musi znajdować się pomiędzy dwiema cyframi. Linia nie może być pusta, musi zawierać przynajmniej jedną liczbę.
* Linia sygnalizująca niemożność poprawnego obliczenia sumy zawiera pięć liter składających się na słowo ERROR (po tych literach jest jeszcze terminator linii, czyli \r\n).
* Serwer nie powinien zamykać połączenia jeśli udało mu się odebrać poprawną linię w sensie ogólnej definicji, ale dane w niej zawarte są niepoprawne. Powinien wtedy zwracać komunikat o błędzie i przechodzić do przetwarzania następnej linii przesłanej przez klienta.
* Serwer powinien zwracać komunikat błędu również wtedy, gdy przesłane przez klienta liczby bądź ich suma przekraczają zakres typu całkowitoliczbowego wykorzystywanego przez serwer do prowadzenia obliczeń.
