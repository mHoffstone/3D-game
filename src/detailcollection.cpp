#include "detailcollection.h"

#include "chunk.h"

DetailCollection::DetailCollection()
{
}

DetailCollection::~DetailCollection()
{
}

void DetailCollection::create(){
	if(m_hasCreated) return;
	for(auto& a : describers){
		a.irMesh.create();
	}
	m_hasCreated = true;
}

void DetailCollection::apply(){
	if(!m_hasCreated) create();
	for(auto& a : describers){
		a.irMesh.apply();
	}
	m_hasApplied = true;
}

bool DetailCollection::hasCreated(){
	return m_hasCreated;
}
bool DetailCollection::hasApplied(){
	return m_hasApplied;
}


int DetailCollection::addDetailType(DetailDescriber describer){
	describers.push_back(describer);
	m_hasCreated = false;
	m_hasApplied = false;
	return 0;
}

const DetailDescriber& DetailCollection::getDescriber(int index) const{
	return describers.at(index);
}
DetailDescriber& DetailCollection::getDescriber(int index){
	return describers.at(index);
}

int DetailCollection::count() const{
	return describers.size();
}

int DetailCollection::addNoiseMap(std::unique_ptr<float[]> map){
	noiseMaps.push_back(std::move(map));
	return noiseMaps.size()-1;
}

float DetailCollection::getNoiseValue(int x, int y, int mapIndex) const{
	return noiseMaps[mapIndex][y*Chunk::SIZE+x];
}

void DetailCollection::render(Renderer& r){
	if(!m_hasApplied) apply();
	for(auto& a : describers){
		a.irMesh.render(r);
	}
}

