#include <iostream>
#include <vector>
#include <cmath>
#include <thread>

#include <chrono>

//#include "tracer.h"

#include "engine/gameengine.h"
#include "engine/renderabletexture.h"
#include "engine/renderablemesh.h"
#include "engine/renderabletexturearray.h"

#include "engine/instancedrenderablemesh.h"

#include "world.h"
#include "entity.h"
#include "player.h"

#include "items.h"
#include "crafting.h"

#include "textrenderer.h"

#include "animation.h"

class Game : public GameEngine{
private:
	RenderableTextureArray textures;

	InstancedRenderableMesh grass;
	RenderableMesh staticMesh;

	RenderableTextureArray tempTextures;
	RenderableMesh tempMesh;

	TextRenderer textRenderer;

	RenderableMesh waterMesh;

	World& world = World::getWorld();

	int fTime = 0;
	std::thread asyncGenerator;

	// Prevoiously golbal static

	std::shared_ptr<Player> player;
	std::shared_ptr<Entity> enemy;

	Program normalProgram;
	int normalProgramId;
	Program waterShader;
	int waterShaderId;
	Uniform timeUniform;
	Program uiProgram;
	int uiProgramId;

	bool f3Toggle = false;

public:
	Game() : GameEngine("Game", 800, 600, true){}
	virtual bool init() override{
		setVSync(true);
		//setTickFrequency(120);
		srand(time(0));
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

// 		glClearColor(0.06f, 0.09f, 0.11f, 1.0f);
		glClearColor(0.8f, 0.9f, 1.0f, 1.0f);


		/*Items::addMesh();
		Items::addMesh();
		Items::addMesh();
		Items::addMesh();
		Items::addMesh();
*/
		Items::addProperty("Something went wrong!");
		Items::addProperty("Fiber");
		Items::addProperty("Organic");
		Items::addProperty("Sharp");
		Items::addProperty("Wood Cutting");
		Items::addProperty("Soil Digging");

		int type;
		type = Items::addItemType("Epic Fail");
		Items::addItemVariation(type, {{0, 100}});

		type = Items::addItemType("Grass");
		Items::setMesh(type, Mesh::createXYPlane({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f}).setTexture(1));
		Items::addItemVariation(type, {
			{Items::getPropertyId("Fiber"), 10},
			{Items::getPropertyId("Organic"), 1}});

		type = Items::addItemType("Rock");
		Items::setMesh(type, Mesh::readObjFile("res/models/rock.obj"));
		Items::addItemVariation(type, {{Items::getPropertyId("Sharp"), 10}});
		Items::addItemVariation(type, {{Items::getPropertyId("Sharp"), 15}});

		type = Items::addItemType("Stick");
		Items::setMesh(type, Mesh::readObjFile("res/models/stick.obj"));
		Items::addItemVariation(type, {});

		type = Items::addItemType("Axe");
		Items::setMesh(type, Mesh::readObjFile("res/models/axe.obj"));
		Items::addItemVariation(type, {{Items::getPropertyId("Wood Cutting"), 10}});

		type = Items::addItemType("Fire Blossom");
		Items::setMesh(type, Mesh::createXZPlane({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f}).setTexture(6).scale(0.5f).translate({0.0f, 0.2f, 0.0f}));
		Items::addItemVariation(type, {});

		type = Items::addItemType("Fire Starter");
		Items::addItemVariation(type, {});

		type = Items::addItemType("Blue Flower");
		Items::setMesh(type, Mesh::createXZPlane({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f}).setTexture(5).scale(0.5f).translate({0.0f, 0.1f, 0.0f}));
		Items::addItemVariation(type, {});

		type = Items::addItemType("Hoe");
		Items::addItemVariation(type, {{Items::getPropertyId("Soil Digging"), 10}});

		type = Items::addItemType("entity_tree");

		type = Items::addItemType("Soil");
		Items::addItemVariation(type, {});

		type = Items::addItemType("Wood");
		Items::addItemVariation(type, {});

		type = Items::addItemType("Wood Slab");
		Items::addItemVariation(type, {});

		type = Items::addItemType("Reeds");
		Items::setMesh(type, Mesh::createXYPlane({0.0f, 0.0f, 0.0f}, {1.0f, 2.0f}).setTexture(7));
		Items::addItemVariation(type, {});

		Crafting::addRecipe(Recipe{
			{
				ItemStack{Items::getItemType("Rock"), -1, 3},
				ItemStack{Items::getItemType("Stick"), -1, 1},
				ItemStack{Items::getItemType("Grass"), -1, 10}},
			{ItemStack{Items::getItemType("Axe"), 0, 1}},
			{
				Items::Modifier{Items::ModifierType::INHERIT_HIGHEST, Items::getPropertyId("Sharp"), 0},
				Items::Modifier{Items::ModifierType::REQUIRE_LEVEL, Items::getPropertyId("Sharp"), 10}
			}
		});
		Crafting::addRecipe(Recipe{
			{
				ItemStack{Items::getItemType("Fire Blossom"), -1, 1},
				ItemStack{Items::getItemType("Grass"), -1, 10},
				ItemStack{Items::getItemType("Stick"), -1, 2}
			},
			{ItemStack{Items::getItemType("Fire Starter"), 0, 1}},
			{}
		});
		Crafting::addRecipe(Recipe{
			{
				ItemStack{Items::getItemType("Rock"), -1, 1},
				ItemStack{Items::getItemType("Stick"), -1, 1},
				ItemStack{Items::getItemType("Grass"), -1, 1}
			},
			{ItemStack{Items::getItemType("Hoe"), 0, 1}},
			{
				Items::Modifier{Items::ModifierType::INHERIT_HIGHEST, Items::getPropertyId("Sharp"), 0},
				Items::Modifier{Items::ModifierType::REQUIRE_LEVEL, Items::getPropertyId("Sharp"), 10}
			}
		});
		Crafting::addRecipe(Recipe{
			{ItemStack{Items::getItemType("Wood"), -1, 1}},
			{ItemStack{Items::getItemType("Wood Slab"), 0, 1}},
			{}
		});

		normalProgram.createProgram();
		Shader normalVertex(GL_VERTEX_SHADER, util::readFile("res/shaders/normal_vertex.glsl"));
		Shader normalGeometry(GL_GEOMETRY_SHADER, util::readFile("res/shaders/normal_geometry.glsl"));
		Shader normalFragment(GL_FRAGMENT_SHADER, util::readFile("res/shaders/normal_fragment.glsl"));
		normalProgram.setShaders({&normalVertex, &normalGeometry, &normalFragment});
		normalProgramId = getRenderer().addProgram(normalProgram);

		waterShader.createProgram();
		Shader waterVertex(GL_VERTEX_SHADER, util::readFile("res/shaders/water_vertex.glsl"));
		Shader waterFragment(GL_FRAGMENT_SHADER, util::readFile("res/shaders/water_fragment.glsl"));
		waterShader.setShaders({&waterVertex, &waterFragment});
		waterShaderId = getRenderer().addProgram(waterShader);
		waterMesh.create();
		waterMesh.add(Mesh::createXZPlane({0.0f, 0.0f, 0.0f}, {80.0f*40, 80.0f*40}));
		waterMesh.textureRepeat(80);
		waterMesh.setTexture(4);
		waterMesh.apply();
		timeUniform.setLocation(waterShader, "u_time");

		/*uiProgram.createProgram();
		Shader uiVertex(GL_VERTEX_SHADER, util::readFile("res/shaders/ui_vertex.glsl"));
		Shader uiFragment(GL_FRAGMENT_SHADER, util::readFile("res/shaders/ui_fragment.glsl"));
		uiProgram.setShaders({&uiVertex, &uiFragment});
		uiProgramId = getRenderer().addProgram(uiProgram);*/

		Grid::init(getRenderer());

		Texture tex0;
		tex0.readImage("res/textures/grass.tga");
		Texture tex1;
		tex1.readImage("res/textures/grass_blades.tga");
		Texture tex2;
		tex2.readImage("res/textures/spruce_texture_2.tga");
		Texture tex3;
		tex3.readImage("res/textures/trunk.tga");
		Texture tex4;
		tex4.readImage("res/textures/water.tga");
		Texture tex5;
		tex5.readImage("res/textures/flower1.tga");
		Texture tex6;
		tex6.readImage("res/textures/flower2.tga");
		Texture tex7;
		tex7.readImage("res/textures/reeds.tga");

		textures.create(tex1.width, tex1.height, GL_NEAREST, GL_CLAMP_TO_EDGE);
		textures.addTexture(tex0);
		textures.addTexture(tex1);
		textures.addTexture(tex2);
		textures.addTexture(tex3);
		textures.addTexture(tex4);
		textures.addTexture(tex5);
		textures.addTexture(tex6);
		textures.addTexture(tex7);
		textures.apply();

		Texture fontSheet;
		fontSheet.readImage("res/textures/font.tga");
		textRenderer.create(fontSheet);

		Texture temp1;
		temp1.readImage("res/textures/trunk.tga");
		tempTextures.create(temp1.width, temp1.height, GL_NEAREST, GL_CLAMP_TO_EDGE);
		tempTextures.addTexture(temp1);
		tempTextures.apply();

		tempMesh.create();
		tempMesh.readObjFile("res/models/axe.obj");
		tempMesh.scale(vec3f{1.4f, 1.4f, 1.4f});
		tempMesh.translate(vec3f{15.0f, world.getHeightAt(15,15)+0.5f, 15.0f});
		tempMesh.apply();

		grass.create();
		grass.add(Mesh::createXYPlane({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f}));
		grass.setTexture(1);

		/*for(int i = 0; i < 10000; i++){
			float x = rand()%10000/10000.0f*10.0f;
			float y = rand()%10000/10000.0f*10.0f;
			grass.instanceData.push_back(Instance{vec3f{x, world.getHeightAt(x, y), y},
				(Matrix4x4::rotationY(rand()%1000/1000.0f * 2*PI)*Matrix4x4::scale(rand()%1000/4000.0f+0.5f)).elements}
				//(Matrix4x4::rotationY(rand()%1000/1000.0f * 2*PI)).elements}
			);
		}*/
		grass.apply();

		//tree.setNormal(vec3f{0.0f, 1.0f, 0.0f});

		staticMesh.create();
		Mesh monkeMesh;
		monkeMesh.readObjFile("res/untitled.obj");
		monkeMesh.setTexture(3);
		staticMesh.apply();

		Animation::init(getRenderer());

		player = std::make_shared<Player>(this);
		player->player = player;
		world.addEntity(player);
		world.player = player;
		player->pos = vec3f{15.0f, 0.0f, 15.0f};
		player->pos.y = world.getHeightAt(player->pos.x, player->pos.z);
		player->oldPos = player->pos;
		//player->radius = 0.25f;
		//player->height = 1.8f;

		player->inventory.give(ItemStack(Items::getItemType("Hoe"), 0, 1));
		player->inventory.give(ItemStack(Items::getItemType("Axe"), 0, 1));
		player->inventory.give(ItemStack(Items::getItemType("Wood Slab"), 0, 1));
		player->inventory.give(ItemStack(Items::getItemType("Rock"), 0, 100));

		enemy = std::make_shared<Entity>();
		enemy->pos = vec3f{0.0f, 0.0f, 0.0f};
		enemy->pos.y = world.getHeightAt(enemy->pos.x, enemy->pos.z);
		enemy->oldPos = enemy->pos;
		enemy->acceleration = 0.0f;
		auto animationRenderer = std::make_shared<AnimationEntityRenderer>(enemy.get());
		enemy->entityRenderer = animationRenderer;
		animationRenderer->animation.setBaseMesh(Mesh::readObjFile("res/models/creature/creature_000001.obj"));
		animationRenderer->animation.addPart(Mesh::readObjFile("res/models/creature/creature_000002.obj"));
		animationRenderer->animation.addPart(Mesh::readObjFile("res/models/creature/creature_000003.obj"));
		animationRenderer->animation.create();
		animationRenderer->animation.apply();
		auto entityAI = std::make_shared<ChaserAI>();
		entityAI->owner = enemy.get();
		enemy->entityAI = entityAI;
		world.addEntity(enemy);

		asyncGenerator = std::thread([this](){
			int s = 0;
			while(isRunning()){
				//while(fTime != 1){}
				/*for(int i = 0; i < 3*Chunk::SIZE; i++){
					world.getChunk(s, i);
				}*/

// 				for(int i = -s; i < s+1; i++){
// 					world.getChunk(-s, -i);
// 				}
// 				for(int i = -s; i < s+1; i++){
// 					world.getChunk(-i, -s);
// 				}
				/*if(s < 80*Chunk::SIZE){
					for(int i = 0; i < s+1; i++){
						world.getChunk(s, i);
					}
					for(int i = 0; i < s+1; i++){
						world.getChunk(i, s);
					}
					s += Chunk::SIZE;
				}*/
				/*for(int z = -10; z <= 10; z++){
					for(int x = -10; x <= 10; x++){
						world.generateGround(player->pos.x + x*Chunk::SIZE/2, player->pos.z + z*Chunk::SIZE/2);
						world.generateDetails(player->pos.x + x*Chunk::SIZE/2, player->pos.z + z*Chunk::SIZE/2, 0);
						if(z*z <= 9 && x*x <= 9){
							world.generateDetails(player->pos.x + x*Chunk::SIZE/2, player->pos.z + z*Chunk::SIZE/2, 1);
						}
					}
				}*/

				fTime = 2;
			}
		});

		float h = world.getHeightAt(20,20);
		world.vPlanes.push_back(std::make_shared<VerticalPlane>(
			vec3f{20.0f, h, 20.0f},
			vec3f{25.0f, h+5, 20.0f}
		));
		world.vPlanes.push_back(std::make_shared<VerticalPlane>(
			vec3f{20.0f, h, 25.0f},
			vec3f{25.0f, h+5, 25.0f}
		));
		world.vPlanes.push_back(std::make_shared<VerticalPlane>(
			vec3f{20.0f, h, 20.0f},
			vec3f{20.0f, h+5, 25.0f}
		));
		world.vPlanes.push_back(std::make_shared<VerticalPlane>(
			vec3f{25.0f, h, 20.0f},
			vec3f{25.0f, h+5, 25.0f}
		));
		world.hPlanes.push_back(std::make_shared<HorizontalPlane>(
			vec3f{20.0f, h+5, 20.0f},
			vec3f{5.0f, 0.0f, 0.0f},
			-5.0f
		));

		for(auto& a : world.vPlanes){a->mesh.setTexture(3); a->mesh.apply();}
		for(auto& a : world.hPlanes){a->mesh.setTexture(3); a->mesh.apply();}

		enableCursor();

		return false;
	}
	int t = 0;
	double tickStartTime, tickEndTime, oldDrawTime;
	bool tick(float dt) override{
		tickStartTime = glfwGetTime();
		dt = 1/60.0f;

		if(getKey(GLFW_KEY_ESCAPE)) return true;

		for(int z = -10; z <= 10; z++){
			for(int x = -10; x <= 10; x++){
				world.generateGround(player->pos.x + x*Chunk::SIZE/2, player->pos.z + z*Chunk::SIZE/2);
				world.generateDetails(player->pos.x + x*Chunk::SIZE/2, player->pos.z + z*Chunk::SIZE/2, 0);
				if(z*z <= 9 && x*x <= 9){
					world.generateDetails(player->pos.x + x*Chunk::SIZE/2, player->pos.z + z*Chunk::SIZE/2, 1);
				}
			}
		}

		//world.generateDetails(player->pos.x, player->pos.z, 1);
		if(getKey(GLFW_KEY_B) == 1){
			world.getChunk(player->pos.x, player->pos.z)->apply();
		}

		if(getKey(GLFW_KEY_V) == 1){
			std::shared_ptr<RigidCube> c = std::make_shared<RigidCube>(1.0f);
			c->setPos(vec3f{10.0f + rand()%100/100.0f, world.getHeightAt(10.0f, 10.0f)+6.0f, 10.0f+rand()%100/100.0f});
			world.cubes.push_back(c);
		}

		/*if(getKey(GLFW_MOUSE_BUTTON_LEFT) && cubes.size() > 0){
			//vec3f delta = vec3f{(float)sin(player->rotation.y), 0.0f, (float)-cos(player->rotation.y)} * cos(player->rotation.x) + vec3f::Y * -sin(player->rotation.x);
			vec3f delta = player->getLookDir();
			vec3f nextPos = player->getEyePos() + delta * 2.0f;
			//ball1->pos = nextPos;
			//cube1.constrainTo(nextPos);
			cubes[0]->points[0].pos = nextPos;
		}*/

		if(getKey(GLFW_KEY_O)){
			enableCursor();
		}
		if(getKey(GLFW_KEY_P)){
			disableCursor();
		}

		if(getKey(GLFW_KEY_F) == 1){
			for(auto a : player->inventory.items){
				util::log(Items::getItemName(a.type), a.count);
			}
		}

		if(getKey(GLFW_KEY_F3) == 1){
			f3Toggle = !f3Toggle;
			if(f3Toggle){
				enableCursor();
			}
			else{
				disableCursor();
			}
		}

		if(!f3Toggle){
			player->processInput(dt, this);
		}
		else{
			if(getKey(GLFW_MOUSE_BUTTON_LEFT) == 1){
				vec2f mpos = getMousePosition();
				mpos *= 2.0f/32.0f;

				int row = (mpos.y-3)/1.1f;
				if(mpos.x >= 21.0f && row >= 0 && row < (int)Crafting::getRecipes().size()){
					if(Crafting::canCraft(player->inventory, Crafting::getRecipe(row))){
						Crafting::craft(player->inventory, Crafting::getRecipe(row));
					}
				}
				else if(mpos.x < 21.0f){
					if(row >= 0 && row < (int)player->inventory.items.size()){
						player->handItem = player->inventory.items.at(row);
					}
					else{
						player->handItem.type = -1;
					}
				}
			}
		}

		world.tick(dt);

		//util::log((std::string)player->pos);


		if(!getKey(GLFW_KEY_C)){
			for(auto& c : world.cubes) c->tickShape(dt);
			for(auto& c : world.cubes) c->accelerate(vec3f::Y * -9.82f);
			for(auto& c : world.cubes) c->tickMovement(dt);
			for(auto& c : world.cubes) c->mesh.setTexture(3);

			for(auto& c : world.cubes){
				c->resolveCollision(dt, world);
				for(auto& a : world.hPlanes) c->resolveCollision(dt, *a);
				for(auto& a : world.vPlanes) c->resolveCollision(dt, *a);
				//if(r2 || r3) c->mesh.setTexture(0);
			}

			for(int i = 0; i < (int)world.cubes.size()-1; i++){
				for(int j = i+1; j < (int)world.cubes.size(); j++){
					if(world.cubes[i]->resolveCollision(dt, world.cubes[j])){
						world.cubes[i]->mesh.setTexture(0);
						world.cubes[j]->mesh.setTexture(0);
					}
				}
			}

		}

		if(t % 60 == 0){
			//util::log("pos", (std::string)player->pos);
			//util::log("vel", (std::string)player->vel);
			//util::log("force", (std::string)player->force);
		}

		t++;
		tickEndTime = glfwGetTime();
		return false;
	}

	void render(Renderer& r) override{
		double drawStartTime = glfwGetTime();
		r.setCameraPosition(player->getEyePos());
		r.setCameraRotation(player->rotation);

		//Entity::renderAll(r);

		r.useProgram(0);
		textures.bindTexture();

		staticMesh.render(r);
		world.render(r);
		grass.render(r);
		//Tree::treeMesh.render(r);

		r.useProgram(0);

		for(auto& c : world.cubes) c->render(r);
		for(auto& a : world.hPlanes) a->render(r);
		for(auto& a : world.vPlanes) a->render(r);

		tempMesh.render(r);

		r.useProgram(waterShaderId);
		textures.bindTexture();
		glUniform1f(timeUniform.getLocation(), t/60.0f);
		waterMesh.render(r);

		r.useProgram(normalProgramId);
		for(auto& c : world.cubes) c->render(r);

		if(!f3Toggle){
			textRenderer.render(r, "vel (km/h): " + std::to_string(60*60/1000 * 60*(player->pos-player->oldPos).length()), {1.0f, 1.0f});
			textRenderer.render(r, "tick ms: " + std::to_string(1000*(tickEndTime-tickStartTime)), {1.0f, 2.0f});
			textRenderer.render(r, "draw ms: " + std::to_string(1000*oldDrawTime), {1.0f, 3.0f});
			textRenderer.render(r, "HP: " + std::to_string(player->hp), {1.0f, 4.0f});
		}

		if(f3Toggle){
			textRenderer.render(r, "Pos: " + (std::string)player->pos, {1.0f, 1.0f});
			if(player->handItem.type != -1) textRenderer.render(r, Items::getItemName(player->handItem.type), {40.0f, 1.0f});
			for(decltype(player->inventory.items.size()) i = 0; i < player->inventory.items.size(); i++){
				ItemStack it = player->inventory.items[i];
				textRenderer.render(r, std::to_string(it.count), {1.0f, 3+i*1.1f});
				textRenderer.render(r, Items::getItemName(it.type), {6.0f, 3+i*1.1f});
			}

			for(decltype(Crafting::getRecipes().size()) i = 0; i < Crafting::getRecipes().size(); i++){
				ItemStack it = Crafting::getRecipe(i).result[0];
				textRenderer.render(r, std::to_string(it.count), {21.0f, 3+i*1.1f});
				textRenderer.render(r, Items::getItemName(it.type), {26.0f, 3+i*1.1f});
			}

			vec2f vpSize;
			vpSize.x = r.getViewportWidth();
			vpSize.y = r.getViewportHeight();
			vec2f mpos = getMousePosition();
			//mpos *= {1.0f/(float)vpSize.x, 1.0f/(float)vpSize.y};
			//mpos += {-1.0f, -1.0f};
			mpos *= 2.0f/32.0f;

			int row = (mpos.y-3)/1.1f;
			if(row >= 0 && row < (int)player->inventory.items.size() && mpos.x < 20.0f){
				auto properties = player->inventory.items[row].getDescriptor().properties;
				for(decltype(properties.size()) i = 0; i < properties.size(); i++){
					textRenderer.render(r, Items::getPropertyName(properties[i].propertyId) + " " + std::to_string(properties[i].level), mpos + vec2f{3.0f, i*1.1f});
				}
			}
			else if(row >= 0 && row < (int)Crafting::getRecipes().size() && mpos.x >= 21.0f){
				auto ingredients = Crafting::getRecipe(row).ingredients;
				for(decltype(ingredients.size()) i = 0; i < ingredients.size(); i++){
					textRenderer.render(r, Items::getItemName(ingredients[i].type) + " " + std::to_string(ingredients[i].count), mpos + vec2f{3.0f, i*1.1f});
				}
			}
		}

		/*RenderableMesh m;
		m.create();
		m.add(Mesh::createSphere(getPhysics(movingPoint)->pos, 0.5f));
		m.apply();
		m.render(r);*/
		oldDrawTime = glfwGetTime()-drawStartTime;
	}

	void exit() override{
		while(!asyncGenerator.joinable()){}
		asyncGenerator.join();
	}
};

int main(){
	Game game;
	game.run();

	/*try{
		//throw std::runtime_error("Epic fail!");
		Game game;
		game.run();
	}
	catch(...){
		printStack();

		throw;
	}*/
	util::logt("Is cleaned", ObjectLife::isCleaned());
	return 0;
}

