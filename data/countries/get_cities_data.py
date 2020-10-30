from requests import get
from bs4 import BeautifulSoup
from django.utils.text import slugify

savepath = "countries_raw/"
source = "https://simplemaps.com"

startpage = get(source + "/resources/free-country-cities")

ignore = {"Canada", "United States"} # These countries got an extra paywall and another csv layout

if startpage.ok:

    soup = BeautifulSoup(startpage.text, features="html5lib")

    countries_div = soup.find(attrs={"class":"col-sm-4"})

    for li in countries_div.find_all("li"):
        link = li.a["href"]
        country = li.a.text.strip()
        if country in ignore:
            continue
        print(country)
        res = get(source + link)
        if not res.ok:
            print("Could not load ", res.url)
            continue
        sub_soup = BeautifulSoup(res.text, features="html5lib")
        csv_link = sub_soup.find("a", text="Download CSV")["href"]
        csv_data = get(source + csv_link)
        if not csv_data.ok:
            print("Could not download ", csv_data.urs)
            continue
        with open(savepath + slugify(country) + ".csv", "wb") as file:
            file.write(csv_data.content)

