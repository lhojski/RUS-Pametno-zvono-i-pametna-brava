# Pametno zvono i pametna brava

![Project Banner](https://via.placeholder.com/800x300?text=Smart+Doorbell+%26+Lock) *(Zamijenite placeholder sa stvarnom slikom projekta)*

## 📌 Opis projekta
Projekt kombinira **pametno zvono** i **pametnu bravu** kako bi pružio moderno rješenje za sigurnost i automatizaciju kućnog ulaza.  
**Cilj:** Poboljšati korisničko iskustvo, osigurati daljinsku kontrolu i praćenje pristupa.

### Ključne značajke:
- 🎥 Video nadzor s detekcijom pokreta  
- 📱 Mobilne obavijesti (Android/iOS)  
- 🔑 Daljinsko otključavanje putem aplikacije  
- 📊 Povijest pristupa (log aktivnosti)  

---

## 🛠️ Funkcijski zahtjevi

### Pametno zvono
| Funkcija                  | Opis                                                                 |
|---------------------------|--------------------------------------------------------------------- |
| Detekcija pokreta         | PIR senzor + kamera snima pokret ispred vrata                        |
| Dvosmjerna komunikacija   | Audio poziv između posjetitelja i korisnika putem aplikacije         |
| Noćni vid                 | IR LED osvjetljenje za rad u mraku                                   |

### Pametna brava
| Funkcija                  | Opis                                                                     |
|---------------------------|-----------------------------------------------------------------------   |
| Bluetooth/Wi-Fi upravljanje | Otključavanje putem mobilne aplikacije                                 |
| Višekorisnički pristup    | Dodjela privremenih pristupa (npr. za goste)                             |
| Emergency mode            | Automatsko otključavanje u slučaju požara (integracija sa senzorom dima) |

---

## 🔧 Tehnologije

### Hardver
- **Mikrokontroler:** ESP32 (Wi-Fi/Bluetooth)  
- **Senzori:** PIR, NFC čitač, touchpad  
- **Kamera:** ESP32-CAM modul  
- **Brava:** Elektromotorski zasun  

### Softver
- **Firmware:** Arduino IDE (C++)  