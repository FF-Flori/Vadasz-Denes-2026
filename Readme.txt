Feladatot beküldő csapat adatai:
    Név: C--
    Iskolája: Irinyi János Református Oktatási Központ
    Felkészítő tanár neve: Borsodi Csaba
    Email cím: filkohaziflorian@irinyi-ref.hu
    Tagok nevei: Filkóházi Flórián, Bondár-Oláh Bence, Hegedűs István András

Programfejlesztői környezetek és használt nyelvek/eszközök verziója:
    Visual Studio Code:
        Verziója: 1.11.0
    Neovim:
        Verziója: 0.11.5
    CLion:
        Verziója: 2025.2.2
    Python:
        Verziója: 3.13.7
    C++:
        Verziója: 17
    CMake:
        Verziója: 4.2.3
    Pybind11:
        Verziója: 3.0.2

Használt külső csomagok:
    Python:
        Dashboard:
            - customtkinter
            - CTkListbox
            - matplotlib

        Szimuláció:
            - pygame
    C++:
        - pybind11

Rövid használati útmutató:

	Az útvonaltervező algoritmust bemutató videó linkje: https://youtu.be/v8WepI7LESs

	Dashboard:
		Dashboardot lehet külön is indítani, nem kell kötelezően a programból indítani.

		Első indítás előtt:
			Érdemes telepíteni a fent felsorolt dolgokat (a külső csomagoknál a dashboard és szimuláció résznél).
			Release-ként letöltött programfájlok esetén a szükséges python csomagok már le vannak töltve az adott rendszerhez.

		Indítás:
			Lehet 2 féleképpen indítani:
			Parancssorban "python dashboard.py" vagy dupla kattintással:
			Ilyenkor egyből a log választó nyílik meg ahol ki lehet választani melyik logot szeretnén megnézni.

			Parancssorban "python dashboard.py " és utána pedig beírva a log nevét (pl. "python dashboard.py "2026.03.19 18.53.0""):
			Ilyenkor egyből már a főablak nyílik meg ahol már látható a kiválasztott log tartalma.

			Használti útmutató:
				Főoldal:
					A log betöltés folyamata (5 másodpercenként automatikusan frissül):
						1. Ellenőrzi, hogy a log fájl mérete változott-e.
						2. Ha igen, akkor ellenőrzi, hogy van-e új sor.
						3. Ha van, akkor betölti az új sort.
					
						Kék "Újratöltés" gomb: Ez annyit csinál hogy manuálisan rá frissít a log betöltésre.
					
						A teljes nézet gombok: Ez annyit csinál hogy az adott diagramot nagyban jelenítik meg, minden adatot mutatva.
					
					Teljes nézet:
						Óra megjelenítése:
						- 32 adat alatt minden időpont látszik.
						- 32 adat felett csak a páros órák jelennek meg.
					
						A diagramok kis méretben 7 darab idő-ig ír ki adatokat.
						A "Gyűjtött Ásványok" diagramnál az érték zárójelben, százalék felül.
					
					Log választó:
						Kék "Újratöltés" gomb: Ez annyit csinál hogy frissíti a log mappát és a listát.
						Itt nincs automatikus időzítő.
						Ha nincs log akkor megjelelenik középen hogy "Sajnos nem található log.".
						Ha van log, akkor megjelenik a logok egy listában. Itt többet egyszerre nem lehet kijelölni.
						Egy log kijelölése után a továbbjutáshoz a kék "Kész" gombra kell nyomni.

	Szimuláció:
		A szimulációt kettő féleképpen lehet elindítani:
			- argumentumokkal
				A program futtatásakor egy argumentumot adhat a felhasználó, ami megadja a szimulációnak, hogy hány óráig tartson. Ennek a számnak 24 és 32767 között kell lennie mindkettő végpontot beleértve.
				Ha a program argumentummal van indítva, akkor a Főmenü után egyből elindul a szimuláció, és nem fogja a felhasználó látni, a "szimulációs idő megadása" ablakot.
			- argumentumok nélkül
				A felhasználónak a szimulációs időt, a főmenü után, a "szimulációs idő megadása" ablakban kellesz megadni. Ebben az esetben fél órákat kell beírni a felhasználónak, szóval ha azt akarja hogy 24 óráig fusson, akkor 48-at kell beírnia. Itt a számnak 48 és 9999 között kell lennie, így a minimum itt is 24 óra.

		A főmenü:
			Itt csak egy gomb van, erre kattintva a felhasználó átléphet a szimulációba, vagy a szimulációs idő megadása ablakba a program futtatásától függően.

		A szimulációs idő megadása ablak:
			Ebben az ablakban lehet megadni a szimulációs időt. Ezt az értéket félórákban kell megadni, de erre fel is hívja a figyelmet egy szöveg. A minimum 48, a maximum pedig 9999, mivel ide max 4 számjegyet lehet beírni.
			A számjegyek megadásához, a számítógép számot jelző billentyűit, és némely esetben a backspace billentyűt kell használni.
			Hogy ebből az ablakból tovább menjünk a szimulációra, meg kell nyomni a képernyő alján található gombot.
			FONTOS: Ez a menü nem fogja jelezni, hogy ha a megadott érték a tartományon kívülre esik. A felső határt (9999) lehetetlen átlépni, viszont ha véletlenül el lett gépelve a megadott érték, és az alsó határ (<48) alá esik, akkor nem fog el indulni a program. Ennek az egyetlen egy vizuális jele, hogy a képernyő nem lesz fekete, ami normál esetben megrörténne a szimuláció elindítása előtt.

		A szimuláció:
			Kontrollok:
				A térképen lehet mozogni fel, balra, le, és jobbra a "W", "A", "S", "D"  billentyűkkel, és a nyilakkal.
				A térképet lehet nagyítani, illetve kicsinyíteni a görgővel.
				Az "E" billentyűvel megnyitható a dashboard.
				FONTOS: Ez csak akkor fog működni, hogy ha a (már részletezett) dashboard által használt csomagokat letöltötte a felhasználó
				A SPACE billentyűvel a kamera egyből visszatér a rover-re, és ott is marad addig amég a billentyű lenyomva van tartva.
				Az Escape és a "p" billentyűvel megállítható a szimuláció. A második megnyomásnál a szimuláció elindul újra
			Felhasználói felület:
				A térkép:
					Az ércek:
						Mindegyik érc fajtának külön sprite-ja van, így könnyen megkűlönböztethetőek.
						Az érc kibányászása után az eltűnik a térképről.
					A rover:
						A rover helyét egy animált rover sprite-ja van.
						Ennek a helyzete mutatja a rover jelenlegi helyzetét a térképen.
					A tervezett útvonal (sárga vonal):
						Ez mutatja a rover jövő beli helyzetét a térképen.
				A képernyőn megjelenő információk:
					Itt a következő információk láthatók:
						- Az eltelt idő
						- A maradék idő
						- Az eddig kiszedett ércek száma típusonként
						- Az akkumulátor töltöttségi szintje
						- A rover sebessége
						- A rover által jelenleg bányászott ásvány
						- A jelenlegi napszak (felül)
						- Egy rövid szöveg ami elmondja milyen betűt kell megnyomni a dashboard megnyitásához
		A szimuláció vége:
			A szimuláció végén a rover visszakerül a kezdő pozíciójára, és ott áll addig, amég a felhasználó be nem zárja az ablakot

