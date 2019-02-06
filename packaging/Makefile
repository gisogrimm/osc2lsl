ALL_CSV=$(wildcard *.csv)

COMMITHASH=$(shell git log -1 --abbrev=7 --pretty='format:%h')
VERSION=$(shell cat ../version)-$(COMMITHASH)-gcc-$(shell gcc -dumpversion)-$(shell lsb_release -si)$(shell lsb_release -sr)
DATE=$(shell LANG=en_EN date "+%d %B %Y")
CODENAME=$(shell lsb_release -sc)

all:
	rm -Rf pack man
	$(MAKE) man/osc2lsl.1.gz
	mkdir -p pack/$(CODENAME)
	for c in $(ALL_CSV); do (cd .. && htchdebian-mkdeb packaging/$$c $(VERSION) ); done
	mv ../*.deb ./pack/$(CODENAME)/
	for c in $(ALL_CSV:.csv=); do ls pack/$(CODENAME)/$${c}_*.deb || return 1; done

ver:
	echo "$(VERSION)"

man/%.1.gz: ../doc/%.1
	mkdir -p man
	cat $< | sed -e "s/-VERSION-/$(VERSION)/g" -e "s/-DATE-/\"$(DATE)\"/g" |gzip > $@
