/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkResampleImageFilter_hxx
#define itkResampleImageFilter_hxx

#include "itkResampleImageFilter.h"
#include "itkObjectFactory.h"
#include "itkIdentityTransform.h"
#include "itkProgressReporter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageScanlineIterator.h"
#include "itkSpecialCoordinatesImage.h"
#include "itkDefaultConvertPixelTraits.h"

namespace itk
{

template< typename TInputImage,
          typename TOutputImage,
          typename TInterpolatorPrecisionType,
          typename TTransformPrecisionType >
ResampleImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType, TTransformPrecisionType >
::ResampleImageFilter() :
  m_Extrapolator( nullptr ),
  m_OutputSpacing( 1.0 ),
  m_OutputOrigin( 0.0 ),
  m_UseReferenceImage( false )
{

  m_Size.Fill( 0 );
  m_OutputStartIndex.Fill( 0 );

  m_OutputDirection.SetIdentity();

  // Pipeline input configuration

  // implicit:
  // #0 "Primary" required

  // #1 "ReferenceImage" optional
  Self::AddRequiredInputName("ReferenceImage",1);
  Self::RemoveRequiredInputName("ReferenceImage");

  // "Transform" required ( not numbered )
  Self::AddRequiredInputName("Transform");
  Self::SetTransform(IdentityTransform< TTransformPrecisionType, ImageDimension >::New());

  m_Interpolator = dynamic_cast< InterpolatorType * >
    ( LinearInterpolatorType::New().GetPointer() );

  m_DefaultPixelValue = NumericTraits<PixelType>::ZeroValue( m_DefaultPixelValue );
  this->DynamicMultiThreadingOn();
}

template< typename TInputImage,
          typename TOutputImage,
          typename TInterpolatorPrecisionType,
          typename TTransformPrecisionType >
void
ResampleImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType, TTransformPrecisionType >
::SetOutputSpacing(const double *spacing)
{
  SpacingType s;
  for(unsigned int i = 0; i < TOutputImage::ImageDimension; ++i)
    {
    s[i] = static_cast< typename SpacingType::ValueType >(spacing[i]);
    }
  this->SetOutputSpacing(s);
}

template< typename TInputImage,
          typename TOutputImage,
          typename TInterpolatorPrecisionType,
          typename TTransformPrecisionType >
void
ResampleImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType, TTransformPrecisionType >
::SetOutputOrigin(const double *origin)
{
  OriginPointType p(origin);

  this->SetOutputOrigin(p);
}

template< typename TInputImage,
          typename TOutputImage,
          typename TInterpolatorPrecisionType,
          typename TTransformPrecisionType >
void
ResampleImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType, TTransformPrecisionType >
::SetOutputParametersFromImage(const ImageBaseType *image)
{
  this->SetOutputOrigin ( image->GetOrigin() );
  this->SetOutputSpacing ( image->GetSpacing() );
  this->SetOutputDirection ( image->GetDirection() );
  this->SetOutputStartIndex ( image->GetLargestPossibleRegion().GetIndex() );
  this->SetSize ( image->GetLargestPossibleRegion().GetSize() );
}

template< typename TInputImage,
          typename TOutputImage,
          typename TInterpolatorPrecisionType,
          typename TTransformPrecisionType >
void
ResampleImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType, TTransformPrecisionType >
::BeforeThreadedGenerateData()
{

  if ( !m_Interpolator )
    {
    itkExceptionMacro(<< "Interpolator not set");
    }

  // Connect input image to interpolator
  m_Interpolator->SetInputImage( this->GetInput() );

  // Connect input image to extrapolator
  if( !m_Extrapolator.IsNull() )
    {
    m_Extrapolator->SetInputImage( this->GetInput() );
    }

  unsigned int nComponents
    = DefaultConvertPixelTraits<PixelType>::GetNumberOfComponents(
        m_DefaultPixelValue );

  if (nComponents == 0)
    {
    PixelComponentType zeroComponent
      = NumericTraits<PixelComponentType>::ZeroValue( zeroComponent );
    nComponents = this->GetInput()->GetNumberOfComponentsPerPixel();
    NumericTraits<PixelType>::SetLength(m_DefaultPixelValue, nComponents );
    for (unsigned int n=0; n<nComponents; n++)
      {
      PixelConvertType::SetNthComponent( n, m_DefaultPixelValue,
                                         zeroComponent );
      }
    }
}

template< typename TInputImage,
          typename TOutputImage,
          typename TInterpolatorPrecisionType,
          typename TTransformPrecisionType >
void
ResampleImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType, TTransformPrecisionType >
::AfterThreadedGenerateData()
{
  // Disconnect input image from the interpolator
  m_Interpolator->SetInputImage(nullptr);
  if( !m_Extrapolator.IsNull() )
    {
    // Disconnect input image from the extrapolator
    m_Extrapolator->SetInputImage(nullptr);
    }
}

template< typename TInputImage,
          typename TOutputImage,
          typename TInterpolatorPrecisionType,
          typename TTransformPrecisionType >
void
ResampleImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType, TTransformPrecisionType >
::DynamicThreadedGenerateData(const OutputImageRegionType & outputRegionForThread)
{
  // Check whether the input or the output is a
  // SpecialCoordinatesImage. If either are, then we cannot use the
  // fast path since index mapping will definitely not be linear.
  using OutputSpecialCoordinatesImageType = SpecialCoordinatesImage< PixelType, ImageDimension >;
  using InputSpecialCoordinatesImageType = SpecialCoordinatesImage< InputPixelType, InputImageDimension >;

  if( outputRegionForThread.GetNumberOfPixels() == 0 )
    {
    return;
    }

  if( outputRegionForThread.GetNumberOfPixels() == 0 )
    {
    return;
    }

  const bool isSpecialCoordinatesImage = ( dynamic_cast< const InputSpecialCoordinatesImageType * >( this->GetInput() )
       || dynamic_cast< const OutputSpecialCoordinatesImageType * >( this->GetOutput() ) );


  // Check whether we can use a fast path for resampling. Fast path
  // can be used if the transformation is linear. Transform respond
  // to the IsLinear() call.
  if ( !isSpecialCoordinatesImage && this->GetTransform()->GetTransformCategory() == TransformType::Linear )
    {
    this->LinearThreadedGenerateData(outputRegionForThread);
    return;
    }

  // Otherwise, we use the normal method where the transform is called
  // for computing the transformation of every point.
  this->NonlinearThreadedGenerateData(outputRegionForThread);
}

template< typename TInputImage,
          typename TOutputImage,
          typename TInterpolatorPrecisionType,
          typename TTransformPrecisionType >
typename ResampleImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType, TTransformPrecisionType >
::PixelType
ResampleImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType, TTransformPrecisionType >
::CastPixelWithBoundsChecking(const InterpolatorOutputType value,
                              const ComponentType minComponent,
                              const ComponentType maxComponent ) const
{
  const unsigned int nComponents = InterpolatorConvertType::GetNumberOfComponents(value);
  PixelType          outputValue;

  NumericTraits<PixelType>::SetLength( outputValue, nComponents );

  for (unsigned int n = 0; n < nComponents; n++)
    {
    ComponentType component = InterpolatorConvertType::GetNthComponent( n, value );

    if ( component < minComponent )
      {
      PixelConvertType::SetNthComponent( n, outputValue, static_cast<PixelComponentType>( minComponent ) );
      }
    else if ( component > maxComponent )
      {
      PixelConvertType::SetNthComponent( n, outputValue, static_cast<PixelComponentType>( maxComponent ) );
      }
    else
      {
      PixelConvertType::SetNthComponent(n, outputValue,
                                        static_cast<PixelComponentType>( component ) );
      }
    }

  return outputValue;
}

template< typename TInputImage,
          typename TOutputImage,
          typename TInterpolatorPrecisionType,
          typename TTransformPrecisionType >
void
ResampleImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType, TTransformPrecisionType >
::NonlinearThreadedGenerateData(const OutputImageRegionType & outputRegionForThread)
{
  OutputImageType *outputPtr = this->GetOutput();
  const InputImageType *inputPtr = this->GetInput();
  const TransformType *transformPtr = this->GetTransform();

  // Honor the SpecialCoordinatesImage isInside value returned
  // by TransformPhysicalPointToContinuousIndex
  using InputSpecialCoordinatesImageType = SpecialCoordinatesImage< InputPixelType, InputImageDimension >;
  const bool isSpecialCoordinatesImage = dynamic_cast< const InputSpecialCoordinatesImageType * >( inputPtr );


  // Create an iterator that will walk the output region for this thread.
  using OutputIterator = ImageRegionIteratorWithIndex< TOutputImage >;
  OutputIterator outIt(outputPtr, outputRegionForThread);

  // Define a few indices that will be used to translate from an input pixel
  // to an output pixel
  PointType outputPoint;         // Coordinates of current output pixel
  PointType inputPoint;          // Coordinates of current input pixel

  ContinuousInputIndexType inputIndex;

  // Min/max values of the output pixel type AND these values
  // represented as the output type of the interpolator
  const PixelComponentType minValue =  NumericTraits< PixelComponentType >::NonpositiveMin();
  const PixelComponentType maxValue =  NumericTraits< PixelComponentType >::max();

  using OutputType = typename InterpolatorType::OutputType;
  const auto minOutputValue = static_cast< ComponentType >( minValue );
  const auto maxOutputValue = static_cast< ComponentType >( maxValue );

  // Walk the output region
  outIt.GoToBegin();

  while ( !outIt.IsAtEnd() )
    {
    // Determine the index of the current output pixel
    outputPtr->TransformIndexToPhysicalPoint(outIt.GetIndex(), outputPoint);

    // Compute corresponding input pixel position
    inputPoint = transformPtr->TransformPoint(outputPoint);
    const bool isInsideInput = inputPtr->TransformPhysicalPointToContinuousIndex(inputPoint, inputIndex);

    PixelType  pixval;
    OutputType value;
    // Evaluate input at right position and copy to the output
    if( m_Interpolator->IsInsideBuffer(inputIndex) && ( !isSpecialCoordinatesImage || isInsideInput ) )
      {
      value = m_Interpolator->EvaluateAtContinuousIndex(inputIndex);
      pixval = this->CastPixelWithBoundsChecking( value, minOutputValue, maxOutputValue );
      outIt.Set(pixval);
      }
    else
      {
      if( m_Extrapolator.IsNull() )
        {
        outIt.Set( m_DefaultPixelValue ); // default background value
        }
      else
        {
        value = m_Extrapolator->EvaluateAtContinuousIndex( inputIndex );
        pixval = this->CastPixelWithBoundsChecking( value, minOutputValue, maxOutputValue );
        outIt.Set(pixval);
        }
      }

    ++outIt;
    }
}

template< typename TInputImage,
          typename TOutputImage,
          typename TInterpolatorPrecisionType,
          typename TTransformPrecisionType >
void
ResampleImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType, TTransformPrecisionType >
::LinearThreadedGenerateData(const OutputImageRegionType & outputRegionForThread)
{
  OutputImageType *outputPtr = this->GetOutput();
  const InputImageType *inputPtr = this->GetInput();
  const TransformType *transformPtr = this->GetTransform();

  // Create an iterator that will walk the output region for this thread.
  using OutputIterator = ImageScanlineIterator< TOutputImage >;
  OutputIterator outIt(outputPtr, outputRegionForThread);

  // Define a few indices that will be used to translate from an input pixel
  // to an output pixel
  PointType outputPoint;         // Coordinates of current output pixel
  PointType inputPoint;          // Coordinates of current input pixel
  PointType tmpOutputPoint;
  PointType tmpInputPoint;

  const InputImageRegionType &largestPossibleRegion = outputPtr->GetLargestPossibleRegion();

  using OutputType = typename InterpolatorType::OutputType;

  // Cache information from the superclass
  PixelType defaultValue = this->GetDefaultPixelValue();

  // Min/max values of the output pixel type AND these values
  // represented as the output type of the interpolator
  const PixelComponentType minValue =  NumericTraits< PixelComponentType >::NonpositiveMin();
  const PixelComponentType maxValue =  NumericTraits< PixelComponentType >::max();

  using OutputType = typename InterpolatorType::OutputType;
  const auto minOutputValue = static_cast< ComponentType >( minValue );
  const auto maxOutputValue = static_cast< ComponentType >( maxValue );

  // As we walk across a scan line in the output image, we trace
  // an oriented/scaled/translated line in the input image. Each scan
  // line has a starting and ending point. Since all transforms
  // are linear, the path between the points is linear and can be
  // defined by interpolating between the two points. By using
  // interpolation we avoid accumulation errors, and by using the
  // whole scan line from the largest possible region we make the
  // computation independent for each point and independent of the
  // region we are processing which makes the method independent of
  // how the whole image is split for processing ( threading,
  // streaming, etc ).
  //

  while ( !outIt.IsAtEnd() )
    {
    // Determine the continuous index of the first and end pixel of output
    // scan line when mapped to the input coordinate frame.

    IndexType index = outIt.GetIndex();
    index[0] = largestPossibleRegion.GetIndex(0);

    ContinuousInputIndexType startIndex;
    outputPtr->TransformIndexToPhysicalPoint(index, outputPoint);
    inputPoint = transformPtr->TransformPoint(outputPoint);
    inputPtr->TransformPhysicalPointToContinuousIndex(inputPoint, startIndex);

    ContinuousInputIndexType endIndex;
    index[0] += largestPossibleRegion.GetSize(0);
    outputPtr->TransformIndexToPhysicalPoint(index, outputPoint);
    inputPoint = transformPtr->TransformPoint(outputPoint);
    inputPtr->TransformPhysicalPointToContinuousIndex(inputPoint, endIndex);

    IndexValueType scanlineIndex = outIt.GetIndex()[0];


    while ( !outIt.IsAtEndOfLine() )
      {

      // Perform linear interpolation between startIndex and endIndex
      const double alpha = (scanlineIndex - largestPossibleRegion.GetIndex(0)) / (double)(largestPossibleRegion.GetSize(0));

      ContinuousInputIndexType inputIndex( startIndex );
      for (unsigned int i = 0; i < ImageDimension; ++i)
        {
        inputIndex[i] += alpha * ( endIndex[i] - startIndex[i] );
        }

      PixelType  pixval;
      OutputType value;
      // Evaluate input at right position and copy to the output
      if ( m_Interpolator->IsInsideBuffer(inputIndex) )
        {
        value = m_Interpolator->EvaluateAtContinuousIndex(inputIndex);
        pixval = this->CastPixelWithBoundsChecking( value, minOutputValue, maxOutputValue );
        outIt.Set(pixval);
        }
      else
        {
        if( m_Extrapolator.IsNull() )
          {
          outIt.Set(defaultValue); // default background value
          }
        else
          {
          value = m_Extrapolator->EvaluateAtContinuousIndex( inputIndex );
          pixval = this->CastPixelWithBoundsChecking( value, minOutputValue, maxOutputValue );
          outIt.Set(pixval);
          }
        }

      ++outIt;
      ++scanlineIndex;
      }
    outIt.NextLine();
    }
}

template< typename TInputImage,
          typename TOutputImage,
          typename TInterpolatorPrecisionType,
          typename TTransformPrecisionType >
void
ResampleImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType, TTransformPrecisionType >
::GenerateInputRequestedRegion()
{
  // Call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();

  if ( !this->GetInput() )
    {
    return;
    }

  // Get pointers to the input and output
  InputImagePointer inputPtr  =
    const_cast< TInputImage * >( this->GetInput() );

  // Determining the actual input region is non-trivial, especially
  // when we cannot assume anything about the transform being used.
  // So we do the easy thing and request the entire input image.
  //
  inputPtr->SetRequestedRegionToLargestPossibleRegion();
}

template< typename TInputImage,
          typename TOutputImage,
          typename TInterpolatorPrecisionType,
          typename TTransformPrecisionType >
void
ResampleImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType, TTransformPrecisionType >
::GenerateOutputInformation()
{
  // Call the superclass' implementation of this method
  Superclass::GenerateOutputInformation();

  // Get pointers to the input and output
  OutputImageType *outputPtr = this->GetOutput();
  if ( !outputPtr )
    {
    return;
    }

  const ReferenceImageBaseType *referenceImage = this->GetReferenceImage();

  // Set the size of the output region
  if ( m_UseReferenceImage && referenceImage )
    {
    outputPtr->SetLargestPossibleRegion(
      referenceImage->GetLargestPossibleRegion() );
    }
  else
    {
    typename TOutputImage::RegionType outputLargestPossibleRegion;
    outputLargestPossibleRegion.SetSize(m_Size);
    outputLargestPossibleRegion.SetIndex(m_OutputStartIndex);
    outputPtr->SetLargestPossibleRegion(outputLargestPossibleRegion);
    }

  // Set spacing and origin
  if ( m_UseReferenceImage && referenceImage )
    {
    outputPtr->SetSpacing( referenceImage->GetSpacing() );
    outputPtr->SetOrigin( referenceImage->GetOrigin() );
    outputPtr->SetDirection( referenceImage->GetDirection() );
    }
  else
    {
    outputPtr->SetSpacing(m_OutputSpacing);
    outputPtr->SetOrigin(m_OutputOrigin);
    outputPtr->SetDirection(m_OutputDirection);
    }
}

template< typename TInputImage,
          typename TOutputImage,
          typename TInterpolatorPrecisionType,
          typename TTransformPrecisionType >
ModifiedTimeType
ResampleImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType, TTransformPrecisionType >
::GetMTime(void) const
{
  ModifiedTimeType latestTime = Object::GetMTime();

  if ( m_Interpolator )
    {
    if ( latestTime < m_Interpolator->GetMTime() )
      {
      latestTime = m_Interpolator->GetMTime();
      }
    }

  return latestTime;
}

template< typename TInputImage,
          typename TOutputImage,
          typename TInterpolatorPrecisionType,
          typename TTransformPrecisionType >
void
ResampleImageFilter< TInputImage, TOutputImage, TInterpolatorPrecisionType, TTransformPrecisionType >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "DefaultPixelValue: "
     << static_cast< typename NumericTraits< PixelType >::PrintType >
  ( m_DefaultPixelValue )
     << std::endl;
  os << indent << "Size: " << m_Size << std::endl;
  os << indent << "OutputStartIndex: " << m_OutputStartIndex << std::endl;
  os << indent << "OutputSpacing: " << m_OutputSpacing << std::endl;
  os << indent << "OutputOrigin: " << m_OutputOrigin << std::endl;
  os << indent << "OutputDirection: " << m_OutputDirection << std::endl;
  os << indent << "Transform: " << this->GetTransform() << std::endl;
  os << indent << "Interpolator: " << m_Interpolator.GetPointer() << std::endl;
  os << indent << "Extrapolator: " << m_Extrapolator.GetPointer() << std::endl;
  os << indent << "UseReferenceImage: " << ( m_UseReferenceImage ? "On" : "Off" )
     << std::endl;
}
} // end namespace itk

#endif