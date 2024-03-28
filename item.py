import sys
import requests
import argparse
import pprint
import re
import json

def getItem(item_id, phase = 1, faction = None, pvp = False):
    if item_id[:4] == "http":
        url = item_id + "&xml"
    else:
        url = "https://www.wowhead.com/classic/?item=" + item_id + "&xml"

    response = requests.get(url)
    if response.status_code != 200:
        print("Error for "+item_id+" - http code: " + response.status_code)
        return None

    xml = response.content.decode()

    p = re.compile("\<error\>(.*?)\<\/error\>")
    m = p.search(xml)
    if m:
        print("Error for "+item_id+" : "+m.group(1))
        return None

    stats = {}
    lvlreq = 0

    # ID
    p = re.compile("\<item id=\"([0-9]+)\"")
    m = p.search(xml)
    if not m:
        print("No id for "+item_id)
        return None
    stats["id"] = int(m.group(1))

    # Name
    p = re.compile("\<name\>\<\!\[CDATA\[([^\]]+)\]\]")
    m = p.search(xml)
    if not m:
        print("No name for "+item_id)
        return None
    stats["title"] = m.group(1)

    # Item level
    p = re.compile("\<level\>([0-9]+)\<")
    m = p.search(xml)
    if m:
        stats["ilvl"] = int(m.group(1))

    # json equip data
    p = re.compile("\<jsonEquip\>\<\!\[CDATA\[(.*?)\]\]\>")
    m = p.search(xml)
    if m:
        equip = json.loads("{"+m.group(1)+"}")
        if "int" in equip:
            stats["int"] = equip["int"]
        if "spi" in equip:
            stats["spi"] = equip["spi"]
        if "splpwr" in equip:
            stats["sp"] = equip["splpwr"]
        if "frosplpwr" in equip:
            stats["sp_frost"] = equip["frosplpwr"]
        if "firsplpwr" in equip:
            stats["sp_fire"] = equip["firsplpwr"]
        if "arcsplpwr" in equip:
            stats["sp_arcane"] = equip["arcsplpwr"]
        if "spldmg" in equip:
            stats["sp"] = equip["spldmg"]
        if "splcritstrkpct" in equip:
            stats["crit"] = equip["splcritstrkpct"]
        elif "rgdcritstrkpct" in equip:
            stats["crit"] = equip["rgdcritstrkpct"]
        if "splhitpct" in equip:
            stats["hit"] = equip["splhitpct"]
        elif "splhitrtng" in equip:
            stats["hit"] = equip["splhitrtng"]
        if "manargn" in equip:
            stats["mp5"] = equip["manargn"]
        if "splpen" in equip:
            stats["spen"] = equip["splpen"]
        if "reqlevel" in equip:
            lvlreq = int(equip["reqlevel"])

    # Two hand
    p = re.compile("\<inventorySlot id=\"([0-9]+)\"")
    m = p.search(xml)
    if m and m.group(1) == "17":
        stats["twohand"] = True

    # Quality
    p = re.compile("\<quality id=\"([0-9]+)\"")
    m = p.search(xml)
    if m and m.group(1) == "3":
        stats["q"] = "rare"
    if m and m.group(1) == "2":
        stats["q"] = "uncommon"

    # Unique
    p = re.compile("Unique")
    m = p.search(xml)
    if m:
        stats["unique"] = True

    # Faction
    if faction != None:
        stats["faction"] = faction

    if pvp == True:
        stats["pvp"] = True

    # Level req
    if lvlreq > 50 and phase < 4:
        phase = 4
    elif lvlreq > 40 and phase < 3:
        phase = 3
    elif lvlreq > 25 and phase < 2:
        phase = 2

    # Phase
    if phase > 1:
        stats["phase"] = phase

    # Convert to item string
    output = json.dumps(stats)
    p = re.compile("\"([a-z0-9_]+)\":")
    output = p.sub(r"\1:", output)
    output = output[:1]+" "+output[1:]
    output = output[:-1]+" "+output[-1:]

    return output

parser = argparse.ArgumentParser()
parser.add_argument("item_id", help="Item ID(s)")
parser.add_argument("-p", help="Phase", type=int, default=1)
parser.add_argument("-f", help="Faction", type=str, default=None)
parser.add_argument("-pvp", help="PvP", action="store_true")
args = parser.parse_args()

ids = args.item_id.split(",")

for index, item_id in enumerate(ids):
    item = getItem(item_id, args.p, args.f, args.pvp)
    if item != None:
        print(item+",")