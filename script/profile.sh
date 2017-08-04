#!/bin/sh

SAMPLES_FILE="tmp/samples"
get_sample() {
  gdb \
    -ex "set confirm off" \
    -ex "set arch i386:x86-64" \
    -ex "file george.multiboot" \
    -ex "target remote localhost:1234" \
    -ex "set height 0"\
    -ex "set logging file $SAMPLES_FILE" \
    -ex "set logging on" \
    -ex "bt" \
    -ex "set logging off" \
    -ex "quit" > /dev/null
  echo "in __END__" >> $SAMPLES_FILE
}

FUCKSTICK='((?<=in )([^ ]+))'
FUCKERSTICK='((?<=at )(.*$))'
FUCKESTSTICK='(?<=#0  )([^0-9][^ ]+)'

FUNC_PAT="$FUCKSTICK|$FUCKESTSTICK"
LINE_PAT="$FUCKSTICK|$FUCKERSTICK|$FUCKESTSTICK"
summarize_funcs() {
  cat $SAMPLES_FILE | grep -Po "$FUNC_PAT" | grep -v '??' | tr '\n' ',' | sed -e 's/,__END__,/\n/g' | cut -d',' -f 1 | sort | uniq -c | sort -n | tac | head -n 10
}

summarize_lines() {
  cat $SAMPLES_FILE | grep -Po "$LINE_PAT" | grep -v '??' | tr '\n' ',' | sed -e 's/,__END__,/\n/g' | cut -d',' -f 1-2 | sed 's/,/ @ /g' | sort | uniq -c | sort -n | tac | head -n 10
}

summarize() {
  cat $SAMPLES_FILE | grep -Po "$FUNC_PAT" | grep -v '??' | tr '\n' ',' | sed -e 's/,__END__,/\n/g' | sort | uniq -c | sort -n | tac | head -n 10
}
mkdir -p tmp
rm -f $SAMPLES_FILE

while true; do
  get_sample
  get_sample
  get_sample
  get_sample
  get_sample
  get_sample
  get_sample
  get_sample
  get_sample
  get_sample
  get_sample
  get_sample
  echo
  echo
  echo
  echo "SUMMARY:"
  echo "BY LINE:"
  summarize_lines
  echo
  echo "BY FUNCTION:"
  summarize_funcs
  echo
  echo "BY FULL STACK:"
  summarize
done
