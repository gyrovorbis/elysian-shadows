local areasToTest = {
	"../../../assets/level/CavesTestArea/area/cavesTest01",
	"../../../assets/level/lorenForest/area/lorenTown",

}


for k, v in pairs(areasToTest) do
	os.execute(arg[1].. " -verify "..v)

end