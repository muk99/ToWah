class GenericEditor : public AudioProcessorEditor,
                      public SliderListener,
                      private Timer
{
public:
    enum
    {
        kParamSliderHeight = 70,
        kParamLabelWidth = 80,
        kParamSliderWidth = 200
    };

    GenericEditor (AudioProcessor& parent)
        : AudioProcessorEditor (parent),
          noParameterLabel ("noparam", "No parameters available")
    {
        const OwnedArray<AudioProcessorParameter>& params = parent.getParameters();
		otherLookAndFeel1.setColour (Slider::thumbColourId, Colours::coral);
		otherLookAndFeel2.setColour (Slider::thumbColourId, Colours::forestgreen);
		
        for (int i = 0; i < params.size(); ++i)
        {
            if (const AudioParameterFloat* param = dynamic_cast<AudioParameterFloat*>(params[i]))
            {
                const bool isLevelMeter = (((param->category & 0xffff0000) >> 16) == 2);
                if (isLevelMeter)
                    continue;

                Slider* aSlider;

                paramSliders.add (aSlider = new Slider (param->name));
                aSlider->setRange (param->range.start, param->range.end);
				
				if (i == params.size()-1){
					aSlider->setSliderStyle (Slider::LinearHorizontal);
					aSlider->setLookAndFeel (&otherLookAndFeel2);
				}
				else { aSlider->setSliderStyle (Slider::Rotary); }
				
				if (i < 3){ aSlider->setLookAndFeel (&otherLookAndFeel1); }
				
                aSlider->setValue (dynamic_cast<const AudioProcessorParameter*>(param)->getValue());

                aSlider->addListener (this);
                addAndMakeVisible (aSlider);

                Label* aLabel;
                paramLabels.add (aLabel = new Label (param->name, param->name));
                addAndMakeVisible (aLabel);
            }
        }

        noParameterLabel.setJustificationType (Justification::horizontallyCentred | Justification::verticallyCentred);
        noParameterLabel.setFont (noParameterLabel.getFont().withStyle (Font::italic));

        setSize ((kParamSliderWidth + kParamLabelWidth)*3,
                 jmax (1, kParamSliderHeight * 3));

        if (paramSliders.size() == 0)
            addAndMakeVisible (noParameterLabel);
        else
            startTimer (100);
    }

    ~GenericEditor()
    {
    }

    void resized() override
    {
        Rectangle<int> r = getLocalBounds();
        noParameterLabel.setBounds (r);
		
		Rectangle<int> sliderArea1 (r.removeFromTop (kParamSliderHeight));
		for (int i = 0; i < 3; ++i)
		{
			Rectangle<int> paramBounds = sliderArea1.removeFromLeft (kParamSliderWidth + kParamLabelWidth);
			Rectangle<int> labelBounds = paramBounds.removeFromLeft (kParamLabelWidth);
			
			paramLabels[i]->setBounds (labelBounds);
			paramSliders[i]->setBounds (paramBounds);
		}
		
		Rectangle<int> sliderArea2 (r.removeFromTop (kParamSliderHeight));
		for (int i = 0; i < 3; ++i)
		{
			Rectangle<int> paramBounds = sliderArea2.removeFromLeft (kParamSliderWidth + kParamLabelWidth);
			Rectangle<int> labelBounds = paramBounds.removeFromLeft (kParamLabelWidth);
			
			paramLabels[i+3]->setBounds (labelBounds);
			paramSliders[i+3]->setBounds (paramBounds);
		}

		Rectangle<int> sliderArea3 = r.removeFromTop (kParamSliderHeight);
		Rectangle<int> labelBounds = sliderArea3.removeFromLeft (kParamLabelWidth);
		
		paramLabels[6]->setBounds (labelBounds);
		paramSliders[6]->setBounds (sliderArea3);
		
	}

    void paint (Graphics& g) override
    {
        g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    }

    //==============================================================================
    void sliderValueChanged (Slider* slider) override
    {
        if (AudioProcessorParameter* param = getParameterForSlider (slider))
        {
            if (slider->isMouseButtonDown())
                param->setValueNotifyingHost ((float) slider->getValue());
            else
                param->setValue ((float) slider->getValue());
        }
    }

    void sliderDragStarted (Slider* slider) override
    {
        if (AudioProcessorParameter* param = getParameterForSlider (slider))
            param->beginChangeGesture();
    }

    void sliderDragEnded (Slider* slider) override
    {
        if (AudioProcessorParameter* param = getParameterForSlider (slider))
            param->endChangeGesture();
    }

private:
    void timerCallback() override
    {
        const OwnedArray<AudioProcessorParameter>& params = getAudioProcessor()->getParameters();
        for (int i = 0; i < params.size(); ++i)
        {
            if (const AudioProcessorParameter* param = params[i])
            {
                if (i < paramSliders.size())
                    paramSliders[i]->setValue (param->getValue());
            }
        }
    }

    AudioProcessorParameter* getParameterForSlider (Slider* slider)
    {
        const OwnedArray<AudioProcessorParameter>& params = getAudioProcessor()->getParameters();
        return params[paramSliders.indexOf (slider)];
    }

    Label noParameterLabel;
    OwnedArray<Slider> paramSliders;
    OwnedArray<Label> paramLabels;
	LookAndFeel_V4 otherLookAndFeel1;
	LookAndFeel_V4 otherLookAndFeel2;
};
