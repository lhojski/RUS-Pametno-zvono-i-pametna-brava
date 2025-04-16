# Pametno zvono i pametna brava

Ovaj projekt kombinira pametno zvono i pametnu bravu s daljinskim upravljanjem. Koristi se Arduino za osnovnu funkcionalnost.

## **Funkcionalnosti**

### **1. Pametno zvono**

| ID       | Funkcionalnost        | Opis                                                            |
| -------- | --------------------- | --------------------------------------------------------------- |
| **FR-1** | Detekcija pokreta     | PIR senzor detektira pokret i aktivira LED osvjetljenje gumba.  |
| **FR-2** | Osvjetljenje gumba    | LED dioda osvijetli gumb nakon detekcije pokreta.               |
| **FR-3** | Zvuk zvona            | Piezo buzzer reproducira zvuk prilikom pritiska na gumb.        |
| **FR-4** | Notifikacija vlasniku | Wi-Fi modul šalje obavijest na mobilni uredaj (Blynk/Telegram). |

### **2. Pametna brava**

| ID       | Funkcionalnost                       | Opis                                                                 |
| -------- | ------------------------------------ | -------------------------------------------------------------------- |
| **FR-6** | Daljinsko otključavanje              | Vlasnik može otključati bravu putem mobilne aplikacije (Blynk/MQTT). |
| **FR-7** | Servo/elektromagnetska brava         | Servo motor ili elektromagnetska brava izvodi fizicko otključavanje. |
| **FR-8** | Automatsko zaključavanje             | Brava se automatski zaključa nakon određenog vremena (npr. 10s).     |
| **FR-9** | Status brave (otključano/zaključano) | LED indikator ili notifikacija prikazuje trenutni status brave.      |

---

## 📌**Potrebne komponente**

| Komponenta            | Količina | Opis                                                           |
| --------------------- | -------- | -------------------------------------------------------------- |
| ESP32                 | 1        | Glavni mikrokontroler.                                         |
| PIR senzor (HC-SR501) | 1        | Detektira pokret i aktivira LED.                               |
| LED dioda             | 1        | Osvjetljava gumb za zvono.                                     |
| Tipka                 | 1        | Za aktivaciju zvona.                                           |
| Piezo buzzer          | 1        | Reproducira zvuk prilikom pritiska na gumb.                    |
| Servo motor (SG90)    | 1        | Simulira mehanizam brave (ili elektromagnetska brava + relej). |
| Breadboard + žice     | -        | Za povezivanje komponenti.                                     |

---

**Autori:** Damjan Jurak i Leon Hojski
