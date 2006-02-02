#! /bin/bash
#
# Create ordinary theme from HiContrast images.
#

SRC="$1"
DST="$2"

if [ -z "$SRC" -o -z "$DST" ] ; then
    echo "Usage: ${0##*/} src dest" 1>&2 
    echo "Create ordinary theme from HiContrast images." 1>&2 
    exit 1
fi

[ "$DST" == "${DST#/}" ] && DST="`pwd`/$DST"
cd "$SRC"

while read WHAT F T ; do
    find . -name "$WHAT" -exec bash -c \
	'FILE={} ; \
	 FNAME="${FILE##*/}" ; \
	 D=${FILE%/*} ; \
	 [ "$D" = "$FILE" ] && D="" ; \
	 D="'"$DST"'/$D" ; \
	 [ -d "$D" ] || mkdir -p "$D" ; \
	 cp "$FILE" "$D/${FNAME/'"$F"'/'"$T"'}"' \;
done << EOF
*_h.png		_h.png	.png
*_sch.png	_sch.png	.png
*_hc.png	_hc.png	.png
lch_*.png	lch_	lc_
sch_*.png	sch_	sc_
lch[0-9]*.png	lch	lc
sch[0-9]*.png	sch	sc
loh[0-9]*.png	loh	lo
lxh[0-9]*.png	lxh	lx
sxh[0-9]*.png	sxh	sx
avh[0-9]*.png	avh	av
avlh[0-9]*.png	avlh	avl
idh[0-9]*.png	idh	id
imh[0-9]*.png	imh	im
mih[0-9]*.png	mih	mi
tbh[0-9]*.png	tbh	tb
nah[0-9]*.png	nah	na
nch[0-9]*.png	nch	nc
nvh[0-9]*.png	nvh	nv
ouh[0-9]*.png	ouh	ou
ddh[0-9]*.png	ddh	dd
sfh[0-9]*.png	sfh	sf
srh[0-9]*.png	srh	sr
wrh[0-9]*.png	wrh	wr
alh[0-9]*.png	alh	al
ath[0-9]*.png	ath	at
bih[0-9]*.png	bih	bi
coh[0-9]*.png	coh	co
foh[0-9]*.png	foh	fo
fuh[0-9]*.png	fuh	fu
oph[0-9]*.png	oph	op
unh[0-9]*.png	unh	un
edh[0-9]*.png	edh	ed
cdh[0-9]*.png	cdh	cd
frh[0-9]*.png	frh	fr
fwh[0-9]*.png	fwh	fw
nuh[0-9]*.png	nuh	nu
prh[0-9]*.png	prh	pr
shh[0-9]*.png	shh	sh
trh[0-9]*.png	trh	tr
reh[0-9]*.png	reh	re
joh[0-9]*.png	joh	jo
fph[0-9]*.png	fph	fp
dah[0-9]*.png	dah	da
EOF
