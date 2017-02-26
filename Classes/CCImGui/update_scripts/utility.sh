function download ()
{
	local link="$1"
	local directory="$2"

	if [[ -z $directory ]]; then
		directory="."
	fi
	wget -N "$link" -O "$directory/$(basename $link)"
}
