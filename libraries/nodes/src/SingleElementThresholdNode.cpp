////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SingleElementThresholdNode.cpp (nodes)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SingleElementThresholdNode.h"
#include "ConstantNode.h"
#include "BinaryPredicateNode.h"

// dataset
#include "DenseDataVector.h"

namespace nodes
{
    SingleElementThresholdNode::SingleElementThresholdNode() : Node({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 1)
    {}

    SingleElementThresholdNode::SingleElementThresholdNode(const model::PortElements<double>& input, const predictors::SingleElementThresholdPredictor& predictor) : Node({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, 1), _predictor(predictor)
    {
        assert(input.Size() > predictor.GetElementIndex());
    }

    utilities::ObjectDescription SingleElementThresholdNode::GetTypeDescription()
    {
        auto description = utilities::MakeObjectDescription<Node, SingleElementThresholdNode>("Single-element threshold predictor node");
        description.template AddProperty<decltype(_input)>("input", "Input port");
        description.template AddProperty<decltype(_output)>("output", "Output port");
        description.template AddProperty<decltype(_predictor)>("predictor", "The predictor");
        return description;
    }

    utilities::ObjectDescription SingleElementThresholdNode::GetDescription() const
    {
        utilities::ObjectDescription description = GetParentDescription<Node, SingleElementThresholdNode>();
        description["input"] = _input;
        description["output"] = _output;
        description["predictor"] = _predictor;
        return description;
    }

    void SingleElementThresholdNode::SetObjectState(const utilities::ObjectDescription& description, utilities::SerializationContext& context)
    {
        Node::SetObjectState(description, context);
        description["input"] >> _input;
        description["output"] >> _output;
        description["predictor"] >> _predictor;
    }
    
    void SingleElementThresholdNode::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<SingleElementThresholdNode>(newPortElements, _predictor);
        transformer.MapNodeOutput(output, newNode->output);
    }

    bool SingleElementThresholdNode::Refine(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());

        // get the element used in the split rule
        model::PortElements<double> element{ newPortElements, _predictor.GetElementIndex() };

        // get the threshold.
        auto thresholdNode = transformer.AddNode<ConstantNode<double>>(_predictor.GetThreshold());

        // create a predicate that implements the split rule
        auto binaryPredicateNode = transformer.AddNode<BinaryPredicateNode<double>>(element, thresholdNode->output, BinaryPredicateNode<double>::PredicateType::greater);

        transformer.MapNodeOutput(output, binaryPredicateNode->output);
        return true;
    }

    void SingleElementThresholdNode::Compute() const
    {
        // create an IDataVector
        dataset::DoubleDataVector dataVector(_input.GetValue());

        // predict
        _output.SetOutput({ _predictor.Predict(dataVector) == 1 ? true : false });
    }

    SingleElementThresholdNode* AddNodeToModelTransformer(const model::PortElements<double>& input, const predictors::SingleElementThresholdPredictor& predictor, model::ModelTransformer& transformer)
    {
        return transformer.AddNode<SingleElementThresholdNode>(input, predictor);
    }
}