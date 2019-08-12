#!/bin/sh

####
# Copyright (C) 2019  AlephObjects, Inc.
#
#
# The bash script in this page is free software: you can
# redistribute it and/or modify it under the terms of the GNU Affero
# General Public License (GNU AGPL) as published by the Free Software
# Foundation, either version 3 of the License, or (at your option)
# any later version.  The code is distributed WITHOUT ANY WARRANTY;
# without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU AGPL for more details.
#

build_config() {
  printer_name=$1
  toolhead_name=$2

  case "$printer_name" in
  *_Experimental)
    PREFIX=EXPERIMENTAL/
    ;;
  *)
    PREFIX=
    ;;
  esac

  ./build-config.py $1 $2 -D ${PREFIX}${printer_name}/${toolhead_name} --summary
}

build_config Gladiola_Mini                      Gladiola_SingleExtruder
build_config Gladiola_Mini                      Albatross_Flexystruder
build_config Gladiola_Mini                      Finch_Aerostruder
build_config Gladiola_Mini                      AchemonSphinx_SmallLayer
build_config Gladiola_Mini                      CecropiaSilk_SingleExtruderAeroV2
build_config Gladiola_Mini                      BandedTiger_HardenedSteel
build_config Gladiola_Mini                      DingyCutworm_HardenedSteelPlus
build_config Gladiola_Mini                      Goldenrod_HardenedExtruder

build_config Gladiola_MiniLCD                   Gladiola_SingleExtruder
build_config Gladiola_MiniLCD                   Albatross_Flexystruder
build_config Gladiola_MiniLCD                   Finch_Aerostruder
build_config Gladiola_MiniLCD                   AchemonSphinx_SmallLayer
build_config Gladiola_MiniLCD                   CecropiaSilk_SingleExtruderAeroV2
build_config Gladiola_MiniLCD                   BandedTiger_HardenedSteel
build_config Gladiola_MiniLCD                   DingyCutworm_HardenedSteelPlus
build_config Gladiola_MiniLCD                   Goldenrod_HardenedExtruder

build_config Hibiscus_Mini2                     AchemonSphinx_SmallLayer
build_config Hibiscus_Mini2                     CecropiaSilk_SingleExtruderAeroV2
build_config Hibiscus_Mini2                     BandedTiger_HardenedSteel
build_config Hibiscus_Mini2                     DingyCutworm_HardenedSteelPlus
build_config Hibiscus_Mini2                     Goldenrod_HardenedExtruder

build_config Juniper_TAZ5                       Tilapia_SingleExtruder
build_config Juniper_TAZ5                       Kanyu_Flexystruder
build_config Juniper_TAZ5                       Opah_Moarstruder
build_config Juniper_TAZ5                       Javelin_DualExtruderV2
build_config Juniper_TAZ5                       Longfin_FlexyDually
build_config Juniper_TAZ5                       Yellowfin_DualExtruderV3
build_config Juniper_TAZ5                       Angelfish_Aerostruder
build_config Juniper_TAZ5                       AchemonSphinx_SmallLayer
build_config Juniper_TAZ5                       CecropiaSilk_SingleExtruderAeroV2
build_config Juniper_TAZ5                       BandedTiger_HardenedSteel
build_config Juniper_TAZ5                       DingyCutworm_HardenedSteelPlus
build_config Juniper_TAZ5                       Goldenrod_HardenedExtruder

build_config Oliveoil_TAZ6                      Tilapia_SingleExtruder
build_config Oliveoil_TAZ6                      Kanyu_Flexystruder
build_config Oliveoil_TAZ6                      Opah_Moarstruder
build_config Oliveoil_TAZ6                      Javelin_DualExtruderV2
build_config Oliveoil_TAZ6                      Longfin_FlexyDually
build_config Oliveoil_TAZ6                      Yellowfin_DualExtruderV3
build_config Oliveoil_TAZ6                      Angelfish_Aerostruder
build_config Oliveoil_TAZ6                      AchemonSphinx_SmallLayer
build_config Oliveoil_TAZ6                      CecropiaSilk_SingleExtruderAeroV2
build_config Oliveoil_TAZ6                      BandedTiger_HardenedSteel
build_config Oliveoil_TAZ6                      DingyCutworm_HardenedSteelPlus
build_config Oliveoil_TAZ6                      Goldenrod_HardenedExtruder

build_config Quiver_TAZPro                      AchemonSphinx_SmallLayer
build_config Quiver_TAZPro                      CecropiaSilk_SingleExtruderAeroV2
build_config Quiver_TAZPro                      BandedTiger_HardenedSteel
build_config Quiver_TAZPro                      DingyCutworm_HardenedSteelPlus
build_config Quiver_TAZPro                      Goldenrod_HardenedExtruder
build_config Quiver_TAZPro                      Quiver_DualExtruder

build_config Redgum_TAZWorkhorse                AchemonSphinx_SmallLayer
build_config Redgum_TAZWorkhorse                CecropiaSilk_SingleExtruderAeroV2
build_config Redgum_TAZWorkhorse                BandedTiger_HardenedSteel
build_config Redgum_TAZWorkhorse                DingyCutworm_HardenedSteelPlus
build_config Redgum_TAZWorkhorse                Goldenrod_HardenedExtruder
build_config Redgum_TAZWorkhorse                Yellowfin_DualExtruderV3

build_config KangarooPaw_Experimental           KangarooPaw_SingleExtruder
build_config HibiscusTouchSD_Experimental       CecropiaSilk_SingleExtruderAeroV2
build_config HibiscusTouchUSB_Experimental      CecropiaSilk_SingleExtruderAeroV2
build_config RedgumArchim_Experimental          CecropiaSilk_SingleExtruderAeroV2
build_config JuniperArchim_Experimental         Tilapia_SingleExtruder
build_config OliveoilArchim_Experimental        Tilapia_SingleExtruder
build_config GladiolaTouchUSB_Experimental      Gladiola_SingleExtruder
build_config GladiolaEinsyLCD_Experimental      Gladiola_SingleExtruder
build_config GladiolaEinsyTouchUSB_Experimental Gladiola_SingleExtruder
build_config CLCDTestStand_Experimental         Gladiola_SingleExtruder