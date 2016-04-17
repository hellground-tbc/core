![logox.png](http://forums.hellground.net/public/style_images/deflection/logo.png)

######################
#   HellGroundCore   #
######################

HUGE thanks to the Hellground.net team for temporarily going open-source with their project in 2014
* Wiki: http://wiki.hellground.net
* Site: https://hellground.net

If the Wiki is ever down, there is a dump of the entire website zipped into the repository.

# Why use HellGroundCore?
The common cores like MaNGOS and TrinityCore were abandoned in 2009, in favor of developing 3.x emulation.
They were later taken on again by different forks like OregonCore and SkyfireOne. Even MaNGOS eventually resumed development.

However, they mostly received some update to their libraries and not many of the quality code base was taken and backported to those forks.
As a result, the public emulators are in a fairly bad state of playability. 
HellGroundCore was developed by the team behind the server for players expecting no gamebreaking bugs. It is fairly stable and a great base - only if you're starting development from scratch.
That's 5 years of development other servers don't have. Yet you shouldn't expect something mind blowing. A lot of the structures are very similar to MaNGOS, as it is based on TrinityCore 2009.
It comes with some custom systems as well as improved debug handling/logging and GM permissions and other smaller features like Recruit A Friend.

# Downsides
* Old libraries, especially since this fork hasn't been updated for 2 years
* 64 bit compile under Visual Study currently doesn't work, needs correct headers
* Not necessarily compatible with fixes ported to cMaNGOS-TBC, OregonCore or TrinityCore2 due to being in closed development for so long


