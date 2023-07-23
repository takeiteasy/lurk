ruby tools/generate.rb
headerdoc2html -pqt wee.h -o docs/
gatherheaderdoc docs/
mv docs/masterTOC.html docs/index.html
