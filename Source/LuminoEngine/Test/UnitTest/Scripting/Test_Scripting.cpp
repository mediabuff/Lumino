
#include <TestConfig.h>
#include <Lumino/Mesh/Mesh.h>
#include "../../Source/Scripting/ScriptingManager.h"
#include "../../Source/Scripting/NlVariant.h"
#include "../../Source/Scripting/NlFunctionNodes.h"


//==============================================================================
class Test_Scripting : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}

};

//------------------------------------------------------------------------------
TEST_F(Test_Scripting, Basic)
{
	auto nlif = RefPtr<NlGraphInterface>::MakeRef();
	nlif->initialize();

	auto epNode = nlif->GetEntryPoint();

	auto node1 = RefPtr<NlNode_Print>::MakeRef();
	auto node2 = RefPtr<NlNode_Add>::MakeRef();

	node2->GetDataInputLhsPin()->GetValueCache()->SetValue(200);
	node2->GetDataInputRhsPin()->GetValueCache()->SetValue(300);

	nlif->GetGraph()->AddGraphNode(node1);
	nlif->GetGraph()->AddGraphNode(node2);

	NlHelper::LinkPins(epNode->GetFlowOutputPin(), node1->GetFlowInputPin());
	NlHelper::LinkPins(node1->GetInputValuePin(), node2->GetDataOutputPin());

	auto ctx = RefPtr<NlContext>::MakeRef();
	//ctx->initialize();
	ctx->CallInterface(nlif);


}

