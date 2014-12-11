/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nsColorControlFrame.h"

#include "nsContentCreatorFunctions.h"
#include "nsContentList.h"
#include "nsContentUtils.h"
#include "nsFormControlFrame.h"
#include "nsGkAtoms.h"
#include "nsIDOMHTMLInputElement.h"
#include "nsIDOMNode.h"
#include "nsIFormControl.h"
#include "nsStyleSet.h"
#include "nsIDocument.h"

using mozilla::dom::Element;

nsColorControlFrame::nsColorControlFrame(nsStyleContext* aContext):
  nsColorControlFrameSuper(aContext)
{
}

nsIFrame*
NS_NewColorControlFrame(nsIPresShell* aPresShell, nsStyleContext* aContext)
{
  return new (aPresShell) nsColorControlFrame(aContext);
}

NS_IMPL_FRAMEARENA_HELPERS(nsColorControlFrame)

NS_QUERYFRAME_HEAD(nsColorControlFrame)
  NS_QUERYFRAME_ENTRY(nsColorControlFrame)
  NS_QUERYFRAME_ENTRY(nsIAnonymousContentCreator)
NS_QUERYFRAME_TAIL_INHERITING(nsColorControlFrameSuper)


void nsColorControlFrame::DestroyFrom(nsIFrame* aDestructRoot)
{
  nsFormControlFrame::RegUnRegAccessKey(static_cast<nsIFrame*>(this), false);
  nsContentUtils::DestroyAnonymousContent(&mColorContent);
  nsColorControlFrameSuper::DestroyFrom(aDestructRoot);
}

nsIAtom*
nsColorControlFrame::GetType() const
{
  return nsGkAtoms::colorControlFrame;
}

#ifdef DEBUG_FRAME_DUMP
nsresult
nsColorControlFrame::GetFrameName(nsAString& aResult) const
{
  return MakeFrameName(NS_LITERAL_STRING("ColorControl"), aResult);
}
#endif

// Create the color area for the button.
// The frame will be generated by the frame constructor.
nsresult
nsColorControlFrame::CreateAnonymousContent(nsTArray<ContentInfo>& aElements)
{
  nsCOMPtr<nsIDocument> doc = mContent->GetComposedDoc();
  mColorContent = doc->CreateHTMLElement(nsGkAtoms::div);

  // Mark the element to be native anonymous before setting any attributes.
  mColorContent->SetIsNativeAnonymousRoot();

  nsresult rv = UpdateColor();
  NS_ENSURE_SUCCESS(rv, rv);

  nsCSSPseudoElements::Type pseudoType = nsCSSPseudoElements::ePseudo_mozColorSwatch;
  nsRefPtr<nsStyleContext> newStyleContext = PresContext()->StyleSet()->
    ResolvePseudoElementStyle(mContent->AsElement(), pseudoType,
                              StyleContext(), mColorContent->AsElement());
  if (!aElements.AppendElement(ContentInfo(mColorContent, newStyleContext))) {
    return NS_ERROR_OUT_OF_MEMORY;
  }

  return NS_OK;
}

void
nsColorControlFrame::AppendAnonymousContentTo(nsTArray<nsIContent*>& aElements,
                                              uint32_t aFilter)
{
  if (mColorContent) {
    aElements.AppendElement(mColorContent);
  }
}

nsresult
nsColorControlFrame::UpdateColor()
{
  // Get the color from the "value" property of our content; it will return the
  // default color (through the sanitization algorithm) if there is none.
  nsAutoString color;
  nsCOMPtr<nsIDOMHTMLInputElement> elt = do_QueryInterface(mContent);
  elt->GetValue(color);
  MOZ_ASSERT(!color.IsEmpty(),
             "Content node's GetValue() should return a valid color string "
             "(the default color, in case no valid color is set)");

  // Set the background-color style property of the swatch element to this color
  return mColorContent->SetAttr(kNameSpaceID_None, nsGkAtoms::style,
      NS_LITERAL_STRING("background-color:") + color, true);
}

nsresult
nsColorControlFrame::AttributeChanged(int32_t  aNameSpaceID,
                                      nsIAtom* aAttribute,
                                      int32_t  aModType)
{
  NS_ASSERTION(mColorContent, "The color div must exist");

  // If the value attribute is set, update the color box, but only if we're
  // still a color control, which might not be the case if the type attribute
  // was removed/changed.
  nsCOMPtr<nsIFormControl> fctrl = do_QueryInterface(GetContent());
  if (fctrl->GetType() == NS_FORM_INPUT_COLOR &&
      aNameSpaceID == kNameSpaceID_None && nsGkAtoms::value == aAttribute) {
    UpdateColor();
  }
  return nsColorControlFrameSuper::AttributeChanged(aNameSpaceID, aAttribute,
                                                    aModType);
}

nsContainerFrame*
nsColorControlFrame::GetContentInsertionFrame()
{
  return this;
}

Element*
nsColorControlFrame::GetPseudoElement(nsCSSPseudoElements::Type aType)
{
  if (aType == nsCSSPseudoElements::ePseudo_mozColorSwatch) {
    return mColorContent;
  }

  return nsContainerFrame::GetPseudoElement(aType);
}
