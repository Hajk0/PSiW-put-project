Montownia

W montowni, na pojedynczym stanowisku, wykonywana jest prosta operacja złożenia produktu końcowego, którego główne komponenty A i B dostarczane są przez
dwóch niezależnych dostawców. Komponenty składowe A i B trafiają do magazynu,
który może pomieścić w sumie N komponentów. Dostawcy w dowolnym momencie
dostarczają komponenty, a ich obiór może być wstrzymany gdy magazyn będzie wypełniony. Montaż polega na: pobraniu z magazynu pojedynczej sztuki komponentu A,
pobraniu pojedynczej sztuki komponentu B i złożeniu ich w produkt końcowy.

Działanie algorytmu:
- inicjacja wszystkich potrzebnych semaforów zmiennych warunkowych oraz magazynu
- stworzenie odpowiedniej liczby producentów produktu a i b oraz stanowisk montażu

- działanie funkcji producenta:
- - wejście do pętli odpowiadającej za wytworzenie odpowiedniej ilości komponentów
- - opuszczenie semafora
- - sprawdzenie czy magazyn jest pełny, jeśli tak oczekiwanie na konsumenta
- - sprawdzenie czy magazyn nie zostanie zapełniony jednym komponentem, jeśli magazyn jest wypełniony jednym komponentem przynajmniej w 95% następuje oczekiwanie na dodanie drugiego typu komponentu
- - dodanie komponentu do magazynu
- - wysłanie sygnału o dodaniu komponentu
- - podniesienie semafora

- działanie funkcji konsumenta:
- - wejście do pętli odpowiadającej za złożenie odpowiedniej ilości komponentów
- - opuszczenie semafora
- - sprawdzenie czy w magazynie znajdują się oba komponenty, jeśli nie oczekiwanie na dodanie brakujących
- - wyjęcie z magazynu ostatniego elementu, sprawdzenie jakiego jest rodzaju i wyszukanie w magazynie brakującego
- - wyjęcie go i przesunięcie elementów które znajdowały się za nim
- - wysłanie sygnału że w magazynie są wolne miejsca
- - podniesienie semafora

- synchronizacja wątków
- usunięcie semaforów i zmiennych warunkowych