/*
 * refit/menu.c
 * Menu functions
 *
 * Copyright (c) 2006 Christoph Pfisterer
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *  * Neither the name of Christoph Pfisterer nor the names of the
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "menu.h"
#include <Platform.h> // Only use angled for Platform, else, xcode project won't compile

#include "../Platform/Settings.h"
#include "../Platform/CloverVersion.h"
#include "menu.h"
#include "screen.h"
#include "../include/Devices.h"
#include "../gui/REFIT_MENU_SCREEN.h"
#include "../Version.h"

#ifndef DEBUG_ALL
#define DEBUG_MENU 1
#else
#define DEBUG_MENU DEBUG_ALL
#endif

#if DEBUG_MENU == 0
#define DBG(...)
#else
#define DBG(...) DebugLog(DEBUG_MENU, __VA_ARGS__)
#endif


INTN LayoutMainMenuHeight = 376;
INTN LayoutAnimMoveForMenuX = 0;

#define TEXT_CORNER_REVISION  (1)
#define TEXT_CORNER_HELP      (2)
#define TEXT_CORNER_OPTIMUS   (3)

REFIT_MENU_ITEM_OPTIONS  MenuEntryOptions (L"Options"_XSW,          1, 0, 'O', ActionEnter);
REFIT_MENU_ITEM_ABOUT    MenuEntryAbout   (L"About BootloaderChooser"_XSW,     1, 0, 'A', ActionEnter);
REFIT_MENU_ITEM_RESET    MenuEntryReset   (L"Restart Computer"_XSW, 1, 0, 'R', ActionSelect);
REFIT_MENU_ITEM_SHUTDOWN MenuEntryShutdown(L"Exit BootloaderChooser"_XSW,      1, 0, 'U', ActionSelect);
REFIT_MENU_ITEM_RETURN   MenuEntryReturn  (L"Return"_XSW,           0, 0,  0,  ActionEnter);

REFIT_MENU_SCREEN MainMenu(NULL, 1, SWPrintf("BootloaderChooser v%s Main Menu", FIRMWARE_REVISION), L"Automatic boot"_XSW);
REFIT_MENU_SCREEN AboutMenu(NULL, 2, L"About"_XSW, L""_XSW);
REFIT_MENU_SCREEN HelpMenu(NULL, 3, L"Help"_XSW, L""_XSW);
REFIT_MENU_SCREEN OptionMenu(NULL, 4, L"Options"_XSW, L""_XSW);





VOID AboutRefit(VOID)
{
  if (AboutMenu.Entries.size() == 0)
  {
    if ( "unknown"_XS8 != LString8(gRevisionStr) ) AboutMenu.AddMenuInfo_f("%s", gRevisionStr);
    if ( "unknown"_XS8 != LString8(gFirmwareBuildDate) ) AboutMenu.AddMenuInfo_f(" Build: %s", gFirmwareBuildDate);
    AboutMenu.AddMenuInfo_f(" ");
    AboutMenu.AddMenuInfo_f("Based on Clover bootloader");
    AboutMenu.AddMenuInfo_f("Portions Copyright (c) Intel Corporation");
    AboutMenu.AddMenuInfo_f("Developers:");
    AboutMenu.AddMenuInfo_f("  jief_machak");
    AboutMenu.AddMenuInfo_f(" ");
    AboutMenu.AddMenuInfo_f("Running on:");
    AboutMenu.AddMenuInfo_f(" EFI Revision %d.%02d",
                                      gST->Hdr.Revision >> 16, gST->Hdr.Revision & ((1 << 16) - 1));
#if defined(MDE_CPU_IA32)
    AboutMenu.AddMenuInfo_f(" Platform: i386 (32 bit)");
#elif defined(MDE_CPU_X64)
    AboutMenu.AddMenuInfo_f(" Platform: x86_64 (64 bit)");
#elif defined(_MSC_VER)
    AboutMenu.AddMenuInfo_f(" Platform: x86_64 (64 bit) VS");
#else
    AboutMenu.AddMenuInfo_f(" Platform: unknown");
#endif
	  AboutMenu.AddMenuInfo_f(" Firmware: %ls rev %d.%04d", gST->FirmwareVendor, gST->FirmwareRevision >> 16, gST->FirmwareRevision & ((1 << 16) - 1));
//	  AboutMenu.AddMenuInfo_f(" Screen Output: %s", egScreenDescription().c_str());
    AboutMenu.AddMenuEntry(&MenuEntryReturn, false);
  }

  AboutMenu.RunMenu(NULL);
}

VOID HelpRefit(VOID)
{
  if (HelpMenu.Entries.size() == 0) {
    switch (gLanguage)
    {
      case russian:
        HelpMenu.AddMenuInfo_f("ESC - Выход из подменю, обновление главного меню");
        HelpMenu.AddMenuInfo_f("F1  - Помощь по горячим клавишам");
        HelpMenu.AddMenuInfo_f("F2  - Сохранить отчет в preboot.log (только если FAT32)");
        HelpMenu.AddMenuInfo_f("F3  - Показать скрытые значки в меню");
        HelpMenu.AddMenuInfo_f("F4  - Родной DSDT сохранить в EFI/CLOVER/ACPI/origin/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F5  - Патченный DSDT сохранить в EFI/CLOVER/ACPI/origin/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F6  - Сохранить ВидеоБиос в EFI/CLOVER/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F7  - Проверить звук на выбранном выходе");
        HelpMenu.AddMenuInfo_f("F8  - Сделать дамп звуковых устройств в EFI/CLOVER/misc/");
        HelpMenu.AddMenuInfo_f("F9  - Изменить разрешение экрана на одно из возможных");
        HelpMenu.AddMenuInfo_f("F10 - Снимок экрана в папку EFI/CLOVER/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F11 - Reset NVRAM");
        HelpMenu.AddMenuInfo_f("F12 - Извлечь указанный DVD");
        HelpMenu.AddMenuInfo_f("Пробел - Дополнительное меню запуска выбранного тома");
        HelpMenu.AddMenuInfo_f("Цифры 1-9 - Быстрый запуск тома по порядку в меню");
        HelpMenu.AddMenuInfo_f("A (About) - О загрузчике");
        HelpMenu.AddMenuInfo_f("O (Options) - Дополнительные настройки");
        HelpMenu.AddMenuInfo_f("R (Reset) - Теплый перезапуск");
        HelpMenu.AddMenuInfo_f("U (go oUt) - Завершить работу в Кловере");
        HelpMenu.AddMenuInfo_f("S (Shell) - Переход в режим командной строки");
        break;
      case ukrainian:
        HelpMenu.AddMenuInfo_f("ESC - Вийти з меню, оновити головне меню");
        HelpMenu.AddMenuInfo_f("F1  - Ця довідка");
        HelpMenu.AddMenuInfo_f("F2  - Зберегти preboot.log (тiльки FAT32)");
        HelpMenu.AddMenuInfo_f("F3  - Відображати приховані розділи");
        HelpMenu.AddMenuInfo_f("F4  - Зберегти OEM DSDT в EFI/CLOVER/ACPI/origin/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F5  - Зберегти патчений DSDT в EFI/CLOVER/ACPI/origin/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F7  - Check sound on selected output");
        HelpMenu.AddMenuInfo_f("F8  - Make audio outputs dump into EFI/CLOVER/misc/");
        HelpMenu.AddMenuInfo_f("F9  - Switch screen resoluton to next possible mode");
        HelpMenu.AddMenuInfo_f("F6  - Зберегти VideoBios в EFI/CLOVER/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F10 - Зберегти знімок екрану в EFI/CLOVER/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F11 - Reset NVRAM");
        HelpMenu.AddMenuInfo_f("F12 - Відкрити обраний диск (DVD)");
        HelpMenu.AddMenuInfo_f("Пробіл - докладніше про обраний пункт меню");
        HelpMenu.AddMenuInfo_f("Клавіші 1-9 -  клавіші пунктів меню");
        HelpMenu.AddMenuInfo_f("A - Про систему");
        HelpMenu.AddMenuInfo_f("O - Опції меню");
        HelpMenu.AddMenuInfo_f("R - Перезавантаження");
        HelpMenu.AddMenuInfo_f("U - Відключити ПК");
        HelpMenu.AddMenuInfo_f("S - Shell");
        break;
      case spanish:
        HelpMenu.AddMenuInfo_f("ESC - Salir de submenu o actualizar el menu principal");
        HelpMenu.AddMenuInfo_f("F1  - Esta Ayuda");
        HelpMenu.AddMenuInfo_f("F2  - Guardar preboot.log (Solo FAT32)");
        HelpMenu.AddMenuInfo_f("F3  - Show hidden entries");
        HelpMenu.AddMenuInfo_f("F4  - Guardar DSDT oem en EFI/CLOVER/ACPI/origin/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F5  - Guardar DSDT parcheado en EFI/CLOVER/ACPI/origin/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F6  - Guardar VideoBios en EFI/CLOVER/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F7  - Check sound on selected output");
        HelpMenu.AddMenuInfo_f("F8  - Make audio outputs dump into EFI/CLOVER/misc/");
        HelpMenu.AddMenuInfo_f("F9  - Switch screen resoluton to next possible mode");
        HelpMenu.AddMenuInfo_f("F10 - Guardar Captura de pantalla en EFI/CLOVER/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F11 - Reset NVRAM");
        HelpMenu.AddMenuInfo_f("F12 - Expulsar volumen seleccionado (DVD)");
        HelpMenu.AddMenuInfo_f("Espacio - Detalles acerca selected menu entry");
        HelpMenu.AddMenuInfo_f("Digitos 1-9 - Atajo a la entrada del menu");
        HelpMenu.AddMenuInfo_f("A - Menu Acerca de");
        HelpMenu.AddMenuInfo_f("O - Menu Optiones");
        HelpMenu.AddMenuInfo_f("R - Reiniciar Equipo");
        HelpMenu.AddMenuInfo_f("U - Apagar");
        HelpMenu.AddMenuInfo_f("S - Shell");
        break;
      case portuguese:
      case brasil:
        HelpMenu.AddMenuInfo_f("ESC - Sai do submenu, atualiza o menu principal");
        HelpMenu.AddMenuInfo_f("F1  - Esta ajuda");
        HelpMenu.AddMenuInfo_f("F2  - Salva preboot.log (somente FAT32)");
        HelpMenu.AddMenuInfo_f("F3  - Show hidden entries");
        HelpMenu.AddMenuInfo_f("F4  - Salva oem DSDT em EFI/CLOVER/ACPI/origin/ (somente FAT32)");
        HelpMenu.AddMenuInfo_f("F5  - Salva DSDT corrigido em EFI/CLOVER/ACPI/origin/ (somente FAT32)");
        HelpMenu.AddMenuInfo_f("F6  - Salva VideoBios em EFI/CLOVER/misc/ (somente FAT32)");
        HelpMenu.AddMenuInfo_f("F7  - Check sound on selected output");
        HelpMenu.AddMenuInfo_f("F8  - Make audio outputs dump into EFI/CLOVER/misc/");
        HelpMenu.AddMenuInfo_f("F9  - Switch screen resoluton to next possible mode");
        HelpMenu.AddMenuInfo_f("F10 - Salva screenshot em EFI/CLOVER/misc/ (somente FAT32)");
        HelpMenu.AddMenuInfo_f("F11 - Reset NVRAM");
        HelpMenu.AddMenuInfo_f("F12 - Ejeta o volume selecionado (DVD)");
        HelpMenu.AddMenuInfo_f("Espaco - Detalhes sobre a opcao do menu selecionada");
        HelpMenu.AddMenuInfo_f("Tecle 1-9 - Atalho para as entradas do menu");
        HelpMenu.AddMenuInfo_f("A - Sobre o Menu");
        HelpMenu.AddMenuInfo_f("O - Opcoes do Menu");
        HelpMenu.AddMenuInfo_f("R - Reiniciar");
        HelpMenu.AddMenuInfo_f("U - Desligar");
        HelpMenu.AddMenuInfo_f("S - Shell");
        break;
      case italian:
        HelpMenu.AddMenuInfo_f("ESC - Esci dal submenu, Aggiorna menu principale");
        HelpMenu.AddMenuInfo_f("F1  - Aiuto");
        HelpMenu.AddMenuInfo_f("F2  - Salva il preboot.log (solo su FAT32)");
        HelpMenu.AddMenuInfo_f("F3  - Mostra volumi nascosti");
        HelpMenu.AddMenuInfo_f("F4  - Salva il DSDT oem in EFI/CLOVER/ACPI/origin/ (solo suFAT32)");
        HelpMenu.AddMenuInfo_f("F5  - Salva il patched DSDT in EFI/CLOVER/ACPI/origin/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F6  - Salva il VideoBios in EFI/CLOVER/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F7  - Controlla il suono sull'uscita selezionata");
        HelpMenu.AddMenuInfo_f("F8  - Scarica le uscite audio in EFI/CLOVER/misc/");
        HelpMenu.AddMenuInfo_f("F9  - Cambia la risoluzione dello schermo alla prossima disponibile");
        HelpMenu.AddMenuInfo_f("F10 - Salva screenshot in EFI/CLOVER/misc/ (solo su FAT32)");
        HelpMenu.AddMenuInfo_f("F11 - Resetta NVRAM");
        HelpMenu.AddMenuInfo_f("F12 - Espelli il volume selezionato (DVD)");
        HelpMenu.AddMenuInfo_f("Spazio - Dettagli sul menu selezionato");
        HelpMenu.AddMenuInfo_f("Digita 1-9 - Abbreviazioni per il menu");
        HelpMenu.AddMenuInfo_f("A - Informazioni");
        HelpMenu.AddMenuInfo_f("O - Menu Opzioni");
        HelpMenu.AddMenuInfo_f("R - Riavvio");
        HelpMenu.AddMenuInfo_f("U - Spegnimento");
        HelpMenu.AddMenuInfo_f("S - Shell");
        break;
      case german:
        HelpMenu.AddMenuInfo_f("ESC - Zurueck aus Untermenue, Hauptmenue erneuern");
        HelpMenu.AddMenuInfo_f("F1  - Diese Hilfe");
        HelpMenu.AddMenuInfo_f("F2  - Sichere preboot.log (nur mit FAT32)");
        HelpMenu.AddMenuInfo_f("F3  - Show hidden entries");
        HelpMenu.AddMenuInfo_f("F4  - Sichere OEM DSDT in EFI/CLOVER/ACPI/origin/ (nur mit FAT32)");
        HelpMenu.AddMenuInfo_f("F5  - Sichere gepatchtes DSDT in EFI/CLOVER/ACPI/origin/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F6  - Sichere VideoBios in EFI/CLOVER/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F7  - Check sound on selected output");
        HelpMenu.AddMenuInfo_f("F8  - Make audio outputs dump into EFI/CLOVER/misc/");
        HelpMenu.AddMenuInfo_f("F9  - Switch screen resoluton to next possible mode");
        HelpMenu.AddMenuInfo_f("F10 - Sichere Bildschirmfoto in EFI/CLOVER/misc/ (nur mit FAT32)");
        HelpMenu.AddMenuInfo_f("F11 - Reset NVRAM");
        HelpMenu.AddMenuInfo_f("F12 - Volume auswerfen (DVD)");
        HelpMenu.AddMenuInfo_f("Leertaste - Details über den gewählten Menue Eintrag");
        HelpMenu.AddMenuInfo_f("Zahlen 1-9 - Kurzwahl zum Menue Eintrag");
        HelpMenu.AddMenuInfo_f("A - Menue Informationen");
        HelpMenu.AddMenuInfo_f("O - Menue Optionen");
        HelpMenu.AddMenuInfo_f("R - Neustart");
        HelpMenu.AddMenuInfo_f("U - Ausschalten");
        HelpMenu.AddMenuInfo_f("S - Shell");
        break;
      case dutch:
        HelpMenu.AddMenuInfo_f("ESC - Verlaat submenu, Vernieuw hoofdmenu");
        HelpMenu.AddMenuInfo_f("F1  - Onderdeel hulp");
        HelpMenu.AddMenuInfo_f("F2  - preboot.log opslaan (Alleen FAT32)");
        HelpMenu.AddMenuInfo_f("F3  - Verborgen opties weergeven");
        HelpMenu.AddMenuInfo_f("F4  - Opslaan oem DSDT in EFI/CLOVER/ACPI/origin/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F5  - Opslaan gepatchte DSDT in EFI/CLOVER/ACPI/origin/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F6  - Opslaan VideoBios in EFI/CLOVER/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F7  - Controleer geluid op geselecteerde uitgang");
        HelpMenu.AddMenuInfo_f("F8  - Opslaan audio uitgangen in EFI/CLOVER/misc/");
        HelpMenu.AddMenuInfo_f("F9  - Wijzig schermresolutie naar eerstvolgende mogelijke modus");
        HelpMenu.AddMenuInfo_f("F10 - Opslaan schermafdruk in EFI/CLOVER/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F11 - Reset NVRAM");
        HelpMenu.AddMenuInfo_f("F12 - Uitwerpen geselecteerd volume (DVD)");
        HelpMenu.AddMenuInfo_f("Spatie - Details over geselecteerd menuoptie");
        HelpMenu.AddMenuInfo_f("Cijfers 1-9 - Snelkoppeling naar menuoptie");
        HelpMenu.AddMenuInfo_f("A - Menu Over");
        HelpMenu.AddMenuInfo_f("O - Menu Opties");
        HelpMenu.AddMenuInfo_f("R - Soft Reset");
        HelpMenu.AddMenuInfo_f("U - Verlaten");
        HelpMenu.AddMenuInfo_f("S - Shell");
        break;
      case french:
        HelpMenu.AddMenuInfo_f("ESC - Quitter sous-menu, Retour menu principal");
        HelpMenu.AddMenuInfo_f("F1  - Aide");
        HelpMenu.AddMenuInfo_f("F2  - Enregistrer preboot.log (FAT32 only)");
        HelpMenu.AddMenuInfo_f("F3  - Show hidden entries");
        HelpMenu.AddMenuInfo_f("F4  - Enregistrer oem DSDT dans EFI/CLOVER/ACPI/origin/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F5  - Enregistrer DSDT modifié dans EFI/CLOVER/ACPI/origin/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F6  - Enregistrer VideoBios dans EFI/CLOVER/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F7  - Check sound on selected output");
        HelpMenu.AddMenuInfo_f("F8  - Make audio outputs dump into EFI/CLOVER/misc/");
        HelpMenu.AddMenuInfo_f("F9  - Switch screen resoluton to next possible mode");
        HelpMenu.AddMenuInfo_f("F10 - Enregistrer la capture d'écran dans EFI/CLOVER/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F11 - Reset NVRAM");
        HelpMenu.AddMenuInfo_f("F12 - Ejecter le volume (DVD)");
        HelpMenu.AddMenuInfo_f("Space - Détails a propos du menu selectionné");
        HelpMenu.AddMenuInfo_f("Digits 1-9 - Raccourci vers entrée menu");
        HelpMenu.AddMenuInfo_f("A - A propos");
        HelpMenu.AddMenuInfo_f("O - Options Menu");
        HelpMenu.AddMenuInfo_f("R - Redémarrer");
        HelpMenu.AddMenuInfo_f("U - Eteindre");
        HelpMenu.AddMenuInfo_f("S - Shell");
        break;
      case indonesian:
        HelpMenu.AddMenuInfo_f("ESC - Keluar submenu, Refresh main menu");
        HelpMenu.AddMenuInfo_f("F1  - Help");
        HelpMenu.AddMenuInfo_f("F2  - Simpan preboot.log ke EFI/CLOVER/ACPI/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F3  - Show hidden entries");
        HelpMenu.AddMenuInfo_f("F4  - Simpan oem DSDT ke EFI/CLOVER/ACPI/origin/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F5  - Simpan patched DSDT ke EFI/CLOVER/ACPI/origin/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F6  - Simpan VideoBios ke EFI/CLOVER/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F7  - Check sound on selected output");
        HelpMenu.AddMenuInfo_f("F8  - Make audio outputs dump into EFI/CLOVER/misc/");
        HelpMenu.AddMenuInfo_f("F9  - Switch screen resoluton to next possible mode");
        HelpMenu.AddMenuInfo_f("F10 - Simpan screenshot ke EFI/CLOVER/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F11 - Reset NVRAM");
        HelpMenu.AddMenuInfo_f("F12 - Eject volume (DVD)");
        HelpMenu.AddMenuInfo_f("Spasi - Detail dari menu yang dipilih");
        HelpMenu.AddMenuInfo_f("Tombol 1-9 - Shortcut pilihan menu");
        HelpMenu.AddMenuInfo_f("A - About");
        HelpMenu.AddMenuInfo_f("O - Opsi");
        HelpMenu.AddMenuInfo_f("R - Soft Reset");
        HelpMenu.AddMenuInfo_f("U - Shutdown");
        HelpMenu.AddMenuInfo_f("S - Shell");
        break;
      case polish:
        HelpMenu.AddMenuInfo_f("ESC - Wyjscie z podmenu, Odswiezenie glownego menu");
        HelpMenu.AddMenuInfo_f("F1  - Pomoc");
        HelpMenu.AddMenuInfo_f("F2  - Zapis preboot.log (tylko FAT32)");
        HelpMenu.AddMenuInfo_f("F3  - Show hidden entries");
        HelpMenu.AddMenuInfo_f("F4  - Zapis DSDT do EFI/CLOVER/ACPI/origin/ (tylko FAT32)");
        HelpMenu.AddMenuInfo_f("F5  - Zapis poprawionego DSDT do EFI/CLOVER/ACPI/origin/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F6  - Zapis BIOSu k. graficznej do EFI/CLOVER/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F7  - Check sound on selected output");
        HelpMenu.AddMenuInfo_f("F8  - Make audio outputs dump into EFI/CLOVER/misc/");
        HelpMenu.AddMenuInfo_f("F9  - Switch screen resoluton to next possible mode");
        HelpMenu.AddMenuInfo_f("F10 - Zapis zrzutu ekranu do EFI/CLOVER/misc/ (tylko FAT32)");
        HelpMenu.AddMenuInfo_f("F11 - Reset NVRAM");
        HelpMenu.AddMenuInfo_f("F12 - Wysuniecie zaznaczonego dysku (tylko dla DVD)");
        HelpMenu.AddMenuInfo_f("Spacja - Informacje nt. dostepnych opcji dla zaznaczonego dysku");
        HelpMenu.AddMenuInfo_f("Znaki 1-9 - Skroty opcji dla wybranego dysku");
        HelpMenu.AddMenuInfo_f("A - Menu Informacyjne");
        HelpMenu.AddMenuInfo_f("O - Menu Opcje");
        HelpMenu.AddMenuInfo_f("R - Restart komputera");
        HelpMenu.AddMenuInfo_f("U - Wylaczenie komputera");
        HelpMenu.AddMenuInfo_f("S - Shell");
        break;
      case croatian:
        HelpMenu.AddMenuInfo_f("ESC - izlaz iz podizbornika, Osvježi glavni izbornik");
        HelpMenu.AddMenuInfo_f("F1  - Ovaj izbornik");
        HelpMenu.AddMenuInfo_f("F2  - Spremi preboot.log (samo na FAT32)");
        HelpMenu.AddMenuInfo_f("F3  - Show hidden entries");
        HelpMenu.AddMenuInfo_f("F4  - Spremi oem DSDT u EFI/CLOVER/ACPI/origin/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F5  - Spremi patched DSDT into EFI/CLOVER/ACPI/origin/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F6  - Spremi VideoBios into EFI/CLOVER/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F7  - Check sound on selected output");
        HelpMenu.AddMenuInfo_f("F8  - Make audio outputs dump into EFI/CLOVER/misc/");
        HelpMenu.AddMenuInfo_f("F9  - Switch screen resoluton to next possible mode");
        HelpMenu.AddMenuInfo_f("F10 - Spremi screenshot into EFI/CLOVER/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F11 - Reset NVRAM");
        HelpMenu.AddMenuInfo_f("F12 - Izbaci izabrai (DVD)");
        HelpMenu.AddMenuInfo_f("Space - Detalji o odabranom sistemu");
        HelpMenu.AddMenuInfo_f("Brojevi 1 do 9 su prečac do izbora");
        HelpMenu.AddMenuInfo_f("A - Izbornik o meni");
        HelpMenu.AddMenuInfo_f("O - Izbornik opcije");
        HelpMenu.AddMenuInfo_f("R - Restart računala");
        HelpMenu.AddMenuInfo_f("U - Isključivanje računala");
        HelpMenu.AddMenuInfo_f("S - Shell");
        break;
      case czech:
        HelpMenu.AddMenuInfo_f("ESC - Vrátit se do hlavní nabídky");
        HelpMenu.AddMenuInfo_f("F1  - Tato Nápověda");
        HelpMenu.AddMenuInfo_f("F2  - Uložit preboot.log (FAT32 only)");
        HelpMenu.AddMenuInfo_f("F3  - Show hidden entries");
        HelpMenu.AddMenuInfo_f("F4  - Uložit oem DSDT do EFI/CLOVER/ACPI/origin/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F5  - Uložit patchnuté DSDT do EFI/CLOVER/ACPI/origin/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F6  - Uložit VideoBios do EFI/CLOVER/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F7  - Check sound on selected output");
        HelpMenu.AddMenuInfo_f("F8  - Make audio outputs dump into EFI/CLOVER/misc/");
        HelpMenu.AddMenuInfo_f("F9  - Switch screen resoluton to next possible mode");
        HelpMenu.AddMenuInfo_f("F10 - Uložit snímek obrazovky do EFI/CLOVER/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F11 - Reset NVRAM");
        HelpMenu.AddMenuInfo_f("F12 - Vysunout vybranou mechaniku (DVD)");
        HelpMenu.AddMenuInfo_f("Mezerník - Podrobnosti o vybraném disku");
        HelpMenu.AddMenuInfo_f("čísla 1-9 - Klávesové zkratky pro disky");
        HelpMenu.AddMenuInfo_f("A - Menu O Programu");
        HelpMenu.AddMenuInfo_f("O - Menu Možnosti");
        HelpMenu.AddMenuInfo_f("R - Částečný restart");
        HelpMenu.AddMenuInfo_f("U - Odejít");
        HelpMenu.AddMenuInfo_f("S - Shell");
        break;
      case korean:
        HelpMenu.AddMenuInfo_f("ESC - 하위메뉴에서 나감, 메인메뉴 새로 고침");
        HelpMenu.AddMenuInfo_f("F1  - 이 도움말");
        HelpMenu.AddMenuInfo_f("F2  - preboot.log를 저장합니다. (FAT32방식에만 해당됨)");
        HelpMenu.AddMenuInfo_f("F3  - Show hidden entries");
        HelpMenu.AddMenuInfo_f("F4  - oem DSDT를 EFI/CLOVER/ACPI/origin/에 저장합니다. (FAT32)");
        HelpMenu.AddMenuInfo_f("F5  - 패치된 DSDT를 EFI/CLOVER/ACPI/origin/에 저장합니다. (FAT32)");
        HelpMenu.AddMenuInfo_f("F6  - VideoBios를 EFI/CLOVER/misc/에 저장합니다. (FAT32)");
        HelpMenu.AddMenuInfo_f("F7  - 선택한 출력에서 사운드 확인");
        HelpMenu.AddMenuInfo_f("F8  - 오디오 코덱덤프를 EFI/CLOVER/misc/에 저장합니다.");
        HelpMenu.AddMenuInfo_f("F9  - Switch screen resoluton to next possible mode");
        HelpMenu.AddMenuInfo_f("F10 - 스크린샷을 EFI/CLOVER/misc/에 저장합니다. (FAT32)");
        HelpMenu.AddMenuInfo_f("F11 - NVRAM 초기화");
        HelpMenu.AddMenuInfo_f("F12 - 선택한 볼륨을 제거합니다. (DVD)");
        HelpMenu.AddMenuInfo_f("Space - 선택한 메뉴의 상세 설명");
        HelpMenu.AddMenuInfo_f("Digits 1-9 - 메뉴 단축 번호");
        HelpMenu.AddMenuInfo_f("A - 단축키 - 이 부트로더에 관하여");
        HelpMenu.AddMenuInfo_f("O - 단축키 - 부트 옵션");
        HelpMenu.AddMenuInfo_f("R - 단축키 - 리셋");
        HelpMenu.AddMenuInfo_f("U - 단축키 - 시스템 종료");
        HelpMenu.AddMenuInfo_f("S - Shell");
        break;
      case romanian:
        HelpMenu.AddMenuInfo_f("ESC - Iesire din sub-meniu, Refresh meniul principal");
        HelpMenu.AddMenuInfo_f("F1  - Ajutor");
        HelpMenu.AddMenuInfo_f("F2  - Salvare preboot.log (doar pentru FAT32)");
        HelpMenu.AddMenuInfo_f("F4  - Salvare oem DSDT in EFI/ACPI/origin/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F5  - Salvare DSDT modificat in EFI/ACPI/origin/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F6  - Salvare VideoBios in EFI/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F7  - Check sound on selected output");
        HelpMenu.AddMenuInfo_f("F8  - Make audio outputs dump into EFI/CLOVER/misc/");
        HelpMenu.AddMenuInfo_f("F9  - Switch screen resoluton to next possible mode");
        HelpMenu.AddMenuInfo_f("F10 - Salvare screenshot in EFI/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F11 - Reset NVRAM");
        HelpMenu.AddMenuInfo_f("F12 - Scoatere volum selectat (DVD)");
        HelpMenu.AddMenuInfo_f("Space - Detalii despre item-ul selectat");
        HelpMenu.AddMenuInfo_f("Cifre 1-9 - Scurtaturi pentru itemele meniului");
        HelpMenu.AddMenuInfo_f("A - Despre");
        HelpMenu.AddMenuInfo_f("O - Optiuni");
        HelpMenu.AddMenuInfo_f("R - Soft Reset");
        HelpMenu.AddMenuInfo_f("U - Inchidere");
        HelpMenu.AddMenuInfo_f("S - Shell");
        break;
      case chinese:
        HelpMenu.AddMenuInfo_f("ESC - 离开子菜单， 刷新主菜单");
        HelpMenu.AddMenuInfo_f("F1  - 帮助");
        HelpMenu.AddMenuInfo_f("F2  - 保存 preboot.log 到 EFI/CLOVER/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F3  - 显示隐藏的启动项");
        HelpMenu.AddMenuInfo_f("F4  - 保存原始的 DSDT 到 EFI/ACPI/origin/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F5  - 保存修正后的 DSDT 到 EFI/ACPI/origin/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F6  - 保存 VideoBios 到 EFI/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F7  - 检查选中输出设备的声音");
        HelpMenu.AddMenuInfo_f("F8  - 生成声卡输出转储到 EFI/CLOVER/misc/");
        HelpMenu.AddMenuInfo_f("F9  - 调整屏幕分辨率为下一个可用的模式");
        HelpMenu.AddMenuInfo_f("F10 - 保存截图到 EFI/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F11 - 重置 NVRAM");
        HelpMenu.AddMenuInfo_f("F12 - 推出选中的卷 (DVD)");
        HelpMenu.AddMenuInfo_f("空格 - 关于选中项的详情");
        HelpMenu.AddMenuInfo_f("数字 1-9 - 菜单快捷键");
        HelpMenu.AddMenuInfo_f("A - 关于");
        HelpMenu.AddMenuInfo_f("O - 选项");
        HelpMenu.AddMenuInfo_f("R - 软复位");
        HelpMenu.AddMenuInfo_f("U - 退出");
        HelpMenu.AddMenuInfo_f("S - Shell");
        break;
      case english:
      default:
        HelpMenu.AddMenuInfo_f("ESC - Escape from submenu, Refresh main menu");
        HelpMenu.AddMenuInfo_f("F1  - This help");
        HelpMenu.AddMenuInfo_f("F2  - Save preboot.log into EFI/CLOVER/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F3  - Show hidden entries");
        HelpMenu.AddMenuInfo_f("F4  - Save oem DSDT into EFI/CLOVER/ACPI/origin/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F5  - Save patched DSDT into EFI/CLOVER/ACPI/origin/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F6  - Save VideoBios into EFI/CLOVER/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F7  - Check sound on selected output");
        HelpMenu.AddMenuInfo_f("F8  - Make audio outputs dump into EFI/CLOVER/misc/");
        HelpMenu.AddMenuInfo_f("F9  - Switch screen resoluton to next possible mode");
        HelpMenu.AddMenuInfo_f("F10 - Save screenshot into EFI/CLOVER/misc/ (FAT32)");
        HelpMenu.AddMenuInfo_f("F11 - Reset NVRAM");
        HelpMenu.AddMenuInfo_f("F12 - Eject selected volume (DVD)");
        HelpMenu.AddMenuInfo_f("Space - Details about selected menu entry");
        HelpMenu.AddMenuInfo_f("Digits 1-9 - Shortcut to menu entry");
        HelpMenu.AddMenuInfo_f("A - Menu About");
        HelpMenu.AddMenuInfo_f("O - Menu Options");
        HelpMenu.AddMenuInfo_f("R - Soft Reset");
        HelpMenu.AddMenuInfo_f("U - Exit from Clover");
        HelpMenu.AddMenuInfo_f("S - Shell");
        break;
    }
    HelpMenu.AddMenuEntry(&MenuEntryReturn, false);
  }

  HelpMenu.RunMenu(NULL);
}

//
// Graphics helper functions
//

/*
  SelectionImages:
    [0] SelectionBig
    [2] SelectionSmall
    [4] SelectionIndicator
  Buttons:
    [0] radio_button
    [1] radio_button_selected
    [2] checkbox
    [3] checkbox_checked
*/
//
// Scrolling functions
//
#define CONSTRAIN_MIN(Variable, MinValue) if (Variable < MinValue) Variable = MinValue
#define CONSTRAIN_MAX(Variable, MaxValue) if (Variable > MaxValue) Variable = MaxValue


//
// user-callable dispatcher functions
//

REFIT_ABSTRACT_MENU_ENTRY* NewEntry_(REFIT_ABSTRACT_MENU_ENTRY *Entry, REFIT_MENU_SCREEN **SubScreen, ACTION AtClick, UINTN ID, const XString8& Title)
{
  Entry->Title = Title;
  Entry->AtClick = AtClick;
  // create the submenu
  *SubScreen = new REFIT_MENU_SCREEN(Entry);
  (*SubScreen)->Title = Entry->Title;
  (*SubScreen)->ID = ID;
  Entry->SubScreen = *SubScreen;
  return Entry;
}

REFIT_MENU_ITEM_OPTIONS* newREFIT_MENU_ITEM_OPTIONS(REFIT_MENU_SCREEN **SubScreen, ACTION AtClick, UINTN ID, const XString8& Title)
{
	REFIT_MENU_ITEM_OPTIONS* Entry = new REFIT_MENU_ITEM_OPTIONS();
	return NewEntry_(Entry, SubScreen, AtClick, ID, Title)->getREFIT_MENU_ITEM_OPTIONS();
}





VOID OptionsMenu(OUT REFIT_ABSTRACT_MENU_ENTRY **ChosenEntry)
{
  REFIT_ABSTRACT_MENU_ENTRY    *TmpChosenEntry = NULL;
  REFIT_ABSTRACT_MENU_ENTRY    *NextChosenEntry = NULL;
  UINTN               MenuExit = 0;
  UINTN               SubMenuExit;
  UINTN               NextMenuExit;
  //CHAR16*           Flags;

  INTN                EntryIndex = 0;
  INTN                SubEntryIndex = -1; //value -1 means old position to remember
  INTN                NextEntryIndex = -1;

  // remember, if you extended this menu then change procedures
  // FillInputs and ApplyInputs

  if (OptionMenu.Entries.size() == 0) {
    OptionMenu.ID = 0;

    REFIT_INPUT_DIALOG *menuItemInputDefaultVolume = OptionMenu.AddMenuItemInput("Default volume: ", TRUE);
    menuItemInputDefaultVolume->Item.ItemType = ASString;
    menuItemInputDefaultVolume->Item.SValue.SWPrintf("%ls", gSettings.DefaultVolume.wc_str());

    REFIT_INPUT_DIALOG *menuItemInputTimeOut = OptionMenu.AddMenuItemInput("Timeout: ", TRUE);
    menuItemInputTimeOut->Item.ItemType = Decimal;
    menuItemInputTimeOut->Item.SValue.SWPrintf("%lld", gSettings.GUITimeOut);

    REFIT_INPUT_DIALOG* menuItemLog = OptionMenu.AddMenuItemInput("Save debug log", false);
    menuItemLog->Item.ItemType = BoolValue;
    menuItemLog->Item.BValue = gSettings.SaveDebugLogToDisk;

//    AddMenuItemInput(&OptionMenu, 90, "Config:", TRUE);
//   InputBootArgs->ShortcutDigit = 0xF1;
//    OptionMenu.AddMenuEntry( SubMenuConfigs(), true);

//    OptionMenu.AddMenuEntry( SubMenuACPI(), true);
//    OptionMenu.AddMenuEntry( SubMenuSmbios(), true);
//    OptionMenu.AddMenuEntry( SubMenuBinaries(), true);
//    OptionMenu.AddMenuEntry( SubMenuQuirks(), true);
//    OptionMenu.AddMenuEntry( SubMenuGraphics(), true);
//    OptionMenu.AddMenuEntry( SubMenuPCI(), true);
//    OptionMenu.AddMenuEntry( SubMenuSpeedStep(), true);
//    OptionMenu.AddMenuEntry( SubMenuAudio(), true);
//    OptionMenu.AddMenuEntry( SubMenuAudioPort(), true);
//    OptionMenu.AddMenuEntry( SubMenuSystem(), true);
    OptionMenu.AddMenuEntry( &MenuEntryReturn, false);
    //DBG("option menu created entries=%d\n", OptionMenu.Entries.size());
  }

  while (!MenuExit) {
    MenuExit = OptionMenu.RunGenericMenu(&EntryIndex, ChosenEntry);
    if (MenuExit == MENU_EXIT_ESCAPE || (*ChosenEntry)->getREFIT_MENU_ITEM_RETURN())
      break;
    if (MenuExit == MENU_EXIT_ENTER || MenuExit == MENU_EXIT_DETAILS) {
      //enter input dialog or subscreen
      if ((*ChosenEntry)->SubScreen != NULL) {
        SubMenuExit = 0;
        while (!SubMenuExit) {
          SubMenuExit = (*ChosenEntry)->SubScreen->RunGenericMenu(&SubEntryIndex, &TmpChosenEntry);
          if (SubMenuExit == MENU_EXIT_ESCAPE || TmpChosenEntry->getREFIT_MENU_ITEM_RETURN()  ){
//            ApplyInputs();
            break;
          }
          if (SubMenuExit == MENU_EXIT_ENTER || MenuExit == MENU_EXIT_DETAILS) {
            if (TmpChosenEntry->SubScreen != NULL) {
              NextMenuExit = 0;
              while (!NextMenuExit) {
                NextMenuExit = TmpChosenEntry->SubScreen->RunGenericMenu(&NextEntryIndex, &NextChosenEntry);
                if (NextMenuExit == MENU_EXIT_ESCAPE || NextChosenEntry->getREFIT_MENU_ITEM_RETURN()  ){
//                  ApplyInputs();
                  break;
                }
                if (NextMenuExit == MENU_EXIT_ENTER || MenuExit == MENU_EXIT_DETAILS) {
                  // enter input dialog
                  NextMenuExit = 0;
//                  ApplyInputs();
                }
              } //while(!NextMenuExit)
            }
            // enter input dialog
            SubMenuExit = 0;
//            ApplyInputs();
          }
        } //while(!SubMenuExit)
      }
      MenuExit = 0;
    } // if MENU_EXIT_ENTER
  }
//exit:

//  ApplyInputs();
}
