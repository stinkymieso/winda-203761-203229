# Projekt 4 - zadanie 3 - winda - Amelia Krysztop 203761, Julia Derlatka 203229

# Ogólny opis projektu

Projekt to prosta symulacja działania windy. Pozwala na kolejkowanie paru żądań oraz na ich optymalizację. Do napisania symulacji użyto języka C++ oraz GDI+ do stworzenia interfejsu graficznego programu.

# Krótki opis działania poszczególnych funkcji w programie

**1. DrawLevitatingPerson() + DrawPersonWithTarget():**

Pozwala na narysowanie prostego lewitującego ludzika z numerem piętra, na które jedzie nad jego głową używając prostych funkcji zawartych w bibliotece GDI+. Pobiera informacje o docelowym położeniu (x oraz y) oraz korzysta ze struktury Person{}; do znalezienia piętra, na które jedzie dany ludzik.

__2. Wholeshaft() + Innershaft():__

Funkcja wholeshaft() rysuje główny szyb windy oraz jej piętra używając współrzędnych okna dialogowego pobranych z funkcji GetClientRect(), których używa do wyśrodkowania windy. W podobny sposób funkcja innershaft() rysuje środkowy szyb windy bazując na współrzędnych okna dialogowego. Szerokość i wysokość windy są z góry ustalone i do nich dopasowane są inne zmienne.
Ze względu na to, że funkcje biblioteki GDI+ typu: DrawRectangle() czy DrawLine() przyjmują tylko wartości typu int albo real, niektóre zmienne musiały zostać inaczej zaimplementowane.
Winda też jest określona strukturą RECT{};, posiada górny i dolny margines (winda nigdy nie sięga samej góry ani samego dołu).

__3. Movement():__

Kontroluje ruch windy, ustawia kierunek jazdy oraz timer pozwalający na animowanie ruchu windy. Ustawia pomocnicze zmienne pickupFloor oraz dropoffFloor, aby winda najpierw pojechała na piętro, z którego został wciśnięty guzik a dopiero potem zabrała osobę na piętro przez nich wybrane guzikiem.

__4. betterQueue():__

Służy jako optymalizacja żądań - bierze kolejkę zawierającą żądania w kolejności wciśnięcia guzików i sprawdza, czy da się zabrać jeszcze kogoś po drodze. Tworzy nową kolejkę, która zawiera zmienioną kolejność wykonywania żądań.

__5. Działanie guzików:__

W WM_CREATE zostają utworzone guziki - każdy z własnym ID, które slużą do przywoływania windy oraz dodania żądania do kolejki. Każde ID jest unikalne i dzięki temu w WM_COMMAND łatwo rozpoznać, z którego piętra oraz na które zostało utworzone żądanie. 
Do pomocy użyta jest także struktura ButtonInfo{};, która odpowiada za przypisanie każdemu guzikowi jego ID, piętra, z którego został wciśnięty oraz piętra, na które ma się udać.

# Krótki opis działania programu

Po wciśnięciu guzika program dodaje żądanie do kolejki oraz tworzy ludzika w środku windy, który porusza się wraz z nią aż do momentu, gdy winda dojedzie na piętro, na którym on chce wysiąść - po tym znika. Przy większej ilości żądań funkcja betterQueue() optymalizuje kolejność wykonania zgłoszeń i tworzy nową kolejkę, której używa do animacji ruchu windy. 





  
   


   

