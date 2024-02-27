// -*- mode:c++; tab-width:2; indent-tabs-mode:nil; c-basic-offset:2 -*-
/*
 * Copyright 2012 ZXing authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * 2012-09-19 HFN translation from Java into C++
 */

#include <zxing/pdf417/decoder/ec/ModulusPoly.h>
#include <zxing/pdf417/decoder/ec/ModulusGF.h>


using zxing::pdf417::decoder::ec::ModulusGF;
using zxing::pdf417::decoder::ec::ModulusPoly;

/**
 * @author Sean Owen
 * @see com.google.zxing.common.reedsolomon.GenericGFPoly
 */

ModulusPoly::ModulusPoly(ModulusGF &field, QSharedPointer<std::vector<int>> coefficients)
    : field_(field)
{
  if (coefficients->size() == 0)
  {
    throw IllegalArgumentException("no coefficients!");
  }
  int coefficientsLength = static_cast<int>(coefficients->size());
  if (coefficientsLength > 1 && (*coefficients)[0] == 0)
  {
    // Leading term must be non-zero for anything except the constant polynomial "0"
    int firstNonZero = 1;
    while (firstNonZero < coefficientsLength && (*coefficients)[firstNonZero] == 0)
    {
      firstNonZero++;
    }
    if (firstNonZero == coefficientsLength)
    {
      coefficientsLength = static_cast<int>(field_.getZero()->getCoefficients()->size());
      coefficients_.reset(new std::vector<int>(coefficientsLength));
      *coefficients_ = *(field_.getZero()->getCoefficients());
    }
    else
    {
      QSharedPointer<std::vector<int>> c(coefficients);
      coefficientsLength -= firstNonZero;
      coefficients_.reset(new std::vector<int>(coefficientsLength));
      for (int i = 0; i < coefficientsLength; i++)
      {
        (*coefficients_)[i] = (*c)[i + firstNonZero];
      }
      /*
        coefficientsLength -= firstNonZero;
        coefficients_.reset(new std::vector<int>(coefficientsLength - firstNonZero));
        for (int i = 0; i < coefficientsLength; i++) {
        coefficients_[i] = coefficients[i + firstNonZero];
        }
      */
    }
  }
  else
  {
    coefficients_ = coefficients;
  }
}

QSharedPointer<std::vector<int>> ModulusPoly::getCoefficients()
{
  return coefficients_;
}

/**
 * @return degree of this polynomial
 */
int ModulusPoly::getDegree()
{
  return static_cast<int>(coefficients_->size()) - 1;
}

/**
 * @return true iff this polynomial is the monomial "0"
 */
bool ModulusPoly::isZero()
{
  return (*coefficients_)[0] == 0;
}

/**
 * @return coefficient of x^degree term in this polynomial
 */
int ModulusPoly::getCoefficient(int degree)
{
  return (*coefficients_)[coefficients_->size() - 1 - degree];
}

/**
 * @return evaluation of this polynomial at a given point
 */
int ModulusPoly::evaluateAt(int a)
{
  int i;
  if (a == 0)
  {
    // Just return the x^0 coefficient
    return getCoefficient(0);
  }
  int size = static_cast<int>(coefficients_->size());
  if (a == 1)
  {
    // Just the sum of the coefficients
    int result = 0;
    for (i = 0; i < size; i++)
    {
      result = field_.add(result, (*coefficients_)[i]);
    }
    return result;
  }
  int result = (*coefficients_)[0];
  for (i = 1; i < size; i++)
  {
    result = field_.add(field_.multiply(a, result), (*coefficients_)[i]);
  }
  return result;
}

QSharedPointer<ModulusPoly> ModulusPoly::add(QSharedPointer<ModulusPoly> other)
{
  if (&field_ != &other->field_)
  {
    throw IllegalArgumentException("ModulusPolys do not have same ModulusGF field");
  }
  if (isZero())
  {
    return other;
  }
  if (other->isZero())
  {
    return QSharedPointer<ModulusPoly>(new ModulusPoly(*this));
  }

  QSharedPointer<std::vector<int>> smallerCoefficients = coefficients_;
  QSharedPointer<std::vector<int>> largerCoefficients = other->coefficients_;
  if (smallerCoefficients->size() > largerCoefficients->size())
  {
    QSharedPointer<std::vector<int>> temp(smallerCoefficients);
    smallerCoefficients = largerCoefficients;
    largerCoefficients = temp;
  }
  QSharedPointer<std::vector<int>> sumDiff(new std::vector<int>(largerCoefficients->size()));
  int lengthDiff = static_cast<int>(largerCoefficients->size() - smallerCoefficients->size());
  // Copy high-order terms only found in higher-degree polynomial's coefficients
  for (int i = 0; i < lengthDiff; i++)
  {
    (*sumDiff)[i] = (*largerCoefficients)[i];
  }

  for (int i = lengthDiff; i < largerCoefficients->size(); i++)
  {
    (*sumDiff)[i] = field_.add((*smallerCoefficients)[i - lengthDiff], (*largerCoefficients)[i]);
  }

  return QSharedPointer<ModulusPoly>(new ModulusPoly(field_, sumDiff));
}

QSharedPointer<ModulusPoly> ModulusPoly::subtract(QSharedPointer<ModulusPoly> other)
{
  if (&field_ != &other->field_)
  {
    throw IllegalArgumentException("ModulusPolys do not have same ModulusGF field");
  }
  if (other->isZero())
  {
    return QSharedPointer<ModulusPoly>(new ModulusPoly(*this));
  }
  return add(other->negative());
}

QSharedPointer<ModulusPoly> ModulusPoly::multiply(QSharedPointer<ModulusPoly> other)
{
  if (&field_ != &other->field_)
  {
    throw IllegalArgumentException("ModulusPolys do not have same ModulusGF field");
  }
  if (isZero() || other->isZero())
  {
    return field_.getZero();
  }
  int i, j;
  QSharedPointer<std::vector<int>> aCoefficients = coefficients_;
  int aLength = static_cast<int>(aCoefficients->size());
  QSharedPointer<std::vector<int>> bCoefficients = other->coefficients_;
  int bLength = static_cast<int>(bCoefficients->size());
  QSharedPointer<std::vector<int>> product(new std::vector<int>(aLength + bLength - 1));
  for (i = 0; i < aLength; i++)
  {
    int aCoeff = (*aCoefficients)[i];
    for (j = 0; j < bLength; j++)
    {
      (*product)[i + j] = field_.add((*product)[i + j], field_.multiply(aCoeff, (*bCoefficients)[j]));
    }
  }
  return QSharedPointer<ModulusPoly>(new ModulusPoly(field_, product));
}

QSharedPointer<ModulusPoly> ModulusPoly::negative()
{
  int size = static_cast<int>(coefficients_->size());
  QSharedPointer<std::vector<int>> negativeCoefficients(new std::vector<int>(size));
  for (int i = 0; i < size; i++)
  {
    (*negativeCoefficients)[i] = field_.subtract(0, (*coefficients_)[i]);
  }
  return QSharedPointer<ModulusPoly>(new ModulusPoly(field_, negativeCoefficients));
}

QSharedPointer<ModulusPoly> ModulusPoly::multiply(int scalar)
{
  if (scalar == 0)
  {
    return field_.getZero();
  }
  if (scalar == 1)
  {
    return QSharedPointer<ModulusPoly>(new ModulusPoly(*this));
  }
  int size = static_cast<int>(coefficients_->size());
  QSharedPointer<std::vector<int>> product(new std::vector<int>(size));
  for (int i = 0; i < size; i++)
  {
    (*product)[i] = field_.multiply((*coefficients_)[i], scalar);
  }
  return QSharedPointer<ModulusPoly>(new ModulusPoly(field_, product));
}

QSharedPointer<ModulusPoly> ModulusPoly::multiplyByMonomial(int degree, int coefficient)
{
  if (degree < 0)
  {
    throw IllegalArgumentException("negative degree!");
  }
  if (coefficient == 0)
  {
    return field_.getZero();
  }
  int size = static_cast<int>(coefficients_->size());
  QSharedPointer<std::vector<int>> product(new std::vector<int>(size + degree));
  for (int i = 0; i < size; i++)
  {
    (*product)[i] = field_.multiply((*coefficients_)[i], coefficient);
  }
  return QSharedPointer<ModulusPoly>(new ModulusPoly(field_, product));
}

std::vector<QSharedPointer<ModulusPoly>> ModulusPoly::divide(QSharedPointer<ModulusPoly> other)
{
  if (&field_ != &other->field_)
  {
    throw IllegalArgumentException("ModulusPolys do not have same ModulusGF field");
  }
  if (other->isZero())
  {
    throw IllegalArgumentException("Divide by 0");
  }

  QSharedPointer<ModulusPoly> quotient(field_.getZero());
  QSharedPointer<ModulusPoly> remainder(new ModulusPoly(*this));

  int denominatorLeadingTerm = other->getCoefficient(other->getDegree());
  int inverseDenominatorLeadingTerm = field_.inverse(denominatorLeadingTerm);

  while (remainder->getDegree() >= other->getDegree() && !remainder->isZero())
  {
    int degreeDifference = remainder->getDegree() - other->getDegree();
    int scale = field_.multiply(remainder->getCoefficient(remainder->getDegree()), inverseDenominatorLeadingTerm);
    QSharedPointer<ModulusPoly> term(other->multiplyByMonomial(degreeDifference, scale));
    QSharedPointer<ModulusPoly> iterationQuotient(field_.buildMonomial(degreeDifference, scale));
    quotient = quotient->add(iterationQuotient);
    remainder = remainder->subtract(term);
  }

  std::vector<QSharedPointer<ModulusPoly>> result(2);
  result[0] = quotient;
  result[1] = remainder;
  return result;
}

#if 0
@Override
public String toString() {
  StringBuilder result = new StringBuilder(8 * getDegree());
  for (int degree = getDegree(); degree >= 0; degree--) {
    int coefficient = getCoefficient(degree);
    if (coefficient != 0) {
      if (coefficient < 0) {
        result.append(" - ");
        coefficient = -coefficient;
      } else {
        if (result.length() > 0) {
          result.append(" + ");
        }
      }
      if (degree == 0 || coefficient != 1) {
        result.append(coefficient);
      }
      if (degree != 0) {
        if (degree == 1) {
          result.append('x');
        } else {
          result.append("x^");
          result.append(degree);
        }
      }
    }
  }
  return result.toString();
}
#endif

ModulusPoly::~ModulusPoly()
{
}
