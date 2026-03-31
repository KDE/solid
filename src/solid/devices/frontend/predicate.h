/*
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_PREDICATE_H
#define SOLID_PREDICATE_H

#include <QSet>
#include <QVariant>

#include <solid/solid_export.h>

#include <solid/deviceinterface.h>

namespace Solid
{
class Device;

/*!
 * \class Solid::Predicate
 * \inheaderfile Solid/Predicate
 * \inmodule Solid
 *
 * \brief This class implements predicates for devices.
 *
 * A predicate is a logical condition that a given device can match or not.
 * It's a constraint about the value a property must have in a given device
 * interface, or any combination (conjunction, disjunction) of such
 * constraints.
 *
 * Predicates are used both in code and in Solid action files
 * (present in \c /usr/share/solid/actions or \c ~/.local/share/solid/actions).
 * When used in Solid action files, they must be present in the \c X-KDE-Solid-Predicate=
 * key in the Desktop Entry section of the file.
 *
 * A predicate can be:
 * \list
 * \li a single comparison
 * \li a conjunction of exactly two predicates ("AND")
 * \li a disjunction of exactly two predicates ("OR")
 * \endlist
 *
 * Since these can be nested, it is possible to express "a StorageVolume
 * that is not ignored AND that StorageVolume contains a FileSystem
 * AND that StorageVolume is removable". Since conjunctions use exactly
 * two predicates (and this expression has three), square brackets are
 * used to group the nested predicates when writing them out in full:
 *
 * \badcode
 *   [ [ StorageVolume.ignored == false AND StorageVolume.usage == 'FileSystem' ]
 *     AND StorageVolume.removable == true ]
 * \endcode
 *
 * Predicates can be constructed programmatically by creating single
 * comparisons with the \l Predicate() constructor, and then building
 * conjunctions with \c operator& and disjunctions with \c operator|.
 *
 * Predicates can be constructed from a string by calling fromString()
 * which returns a predicate. If there
 * are any errors in parsing the string, an empty predicate is returned;
 * use isValid() to detect whether that is the case.
 *
 * The string language is described exactly in
 * \l {https://invent.kde.org/frameworks/solid/-/blob/master/src/solid/devices/predicate_parser.y}
 * {predicate_parser.y},
 * but boils down to:
 * \list
 * \li a single comparison is written as \c {<interface>.<property> == <value>}
 * \li a single bitmask check is written as \c {<interface>.<property> & <value>}
 * \li a conjunction is written as \c {[ <predicate> AND <predicate> ]}
 * \li a disjunction is written as \c {[ <predicate> OR <predicate> ]}
 * \endlist
 * Note the mandatory use of \c {[} and \c {]} around conjunctions and disjunctions.
 * \sa {https://develop.kde.org/docs/features/solid/}{Solid tutorial}
 */
class SOLID_EXPORT Predicate
{
public:
    /*!
     * The comparison operator which can be used for matching within the predicate.
     *
     * \value Equals The property and the value will match for strict equality
     * \value Mask The property and the value will match if the bitmasking is not null
     */
    enum ComparisonOperator { Equals, Mask };

    /*!
     * The predicate type which controls how the predicate is handled.
     *
     * \value PropertyCheck The predicate contains a comparison that needs to be matched using a ComparisonOperator
     * \value Conjunction The two contained predicates need to be true for this predicate to be true
     * \value Disjunction Either of the two contained predicates may be true for this predicate to be true
     * \value InterfaceCheck The device type is compared
     */
    enum Type { PropertyCheck, Conjunction, Disjunction, InterfaceCheck };

    /*!
     * Constructs an invalid predicate.
     */
    Predicate();

    Predicate(const Predicate &other);

    /*!
     * Constructs a predicate matching the \a value of a \a property in
     * a given device interface \a ifaceType.
     *
     * The optional \a compOperator applies between the property and the value when matching.
     */
    Predicate(const DeviceInterface::Type &ifaceType, const QString &property, const QVariant &value, ComparisonOperator compOperator = Equals);

    /*!
     * Constructs a predicate matching the \a value of a \a property in
     * a given device interface \a ifaceType.
     *
     * The optional \a compOperator applies between the property and the value when matching.
     */
    Predicate(const QString &ifaceName, const QString &property, const QVariant &value, ComparisonOperator compOperator = Equals);

    /*!
     * Constructs a predicate matching devices of a particular device interface \a ifaceType.
     */
    explicit Predicate(const DeviceInterface::Type &ifaceType);

    /*!
     * Constructs a predicate matching devices of a particular device interface \a ifaceName.
     */
    explicit Predicate(const QString &ifaceName);

    /*!
     * Argument for the \l Predicate() constructor.
     *
     * \value NOT
     */
    enum NotOperator {
        NOT
    };

    /*!
     * Constructs a predicate that checks that a device does \b not have the \a ifaceName.
     *
     * The operator \a op is always NOT.
     * \since 6.17
     */
    explicit Predicate(QStringView ifaceName, NotOperator op);

    ~Predicate();

    Predicate &operator=(const Predicate &other);

    /*!
     * 'And' operator.
     *
     * Returns a new 'and' predicate having this and \a other as operands.
     */
    Predicate operator&(const Predicate &other);

    /*!
     * 'AndEquals' operator.
     *
     * Assigns to this a new 'and' predicate having this and \a other as operands.
     */
    Predicate &operator&=(const Predicate &other);

    /*!
     * 'Or' operator.
     *
     * Returns a new 'or' predicate having this and \a other as operands.
     */
    Predicate operator|(const Predicate &other);

    /*!
     * 'OrEquals' operator.
     *
     * Assigns to this a new 'or' predicate having this and \a other as operands.
     */
    Predicate &operator|=(const Predicate &other);

    /*!
     * Returns if the predicate is valid.
     *
     * Predicate() is the only invalid predicate.
     */
    bool isValid() const;

    /*!
     * Returns if a \a device matches the predicate.
     */
    bool matches(const Device &device) const;

    /*!
     * Returns the device interface types used in this predicate.
     */
    QSet<DeviceInterface::Type> usedTypes() const;

    /*!
     * Returns a string representation of the predicate.
     */
    QString toString() const;

    /*!
     * Returns a new valid predicate if the given \a predicate string is syntactically
     * correct, Predicate() otherwise.
     * \code
     * Solid::Predicate equalPredicate =
     *     Solid::Predicate::fromString("IS OpticalDisc");
     * \endcode
     */
    static Predicate fromString(const QString &predicate);

    /*!
     * Retrieves the predicate type, used to determine how to handle the predicate.
     * \since 4.4
     */
    Type type() const;

    /*!
     * Returns a device interface type used by the predicate.
     * \note This is only valid for InterfaceCheck and PropertyCheck types.
     * \since 4.4
     */
    DeviceInterface::Type interfaceType() const;

    /*!
     * Returns the property name used when retrieving the value to compare against.
     * \note This is only valid for the PropertyCheck type.
     * \since 4.4
     */
    QString propertyName() const;

    /*!
     * Returns the value used when comparing a devices property to see if it matches the predicate.
     * \note This is only valid for the PropertyCheck type.
     * \since 4.4
     */
    QVariant matchingValue() const;

    /*!
     * Returns the comparison operator used to compare a property's value.
     * \since 4.4
     * \note This is only valid for Conjunction and Disjunction types.
     */
    ComparisonOperator comparisonOperator() const;

    /*!
     * A smaller, inner predicate which is the first to appear and is compared
     * with the second one in a comparison operation.
     *
     * \since 4.4
     * \note This is only valid for Conjunction and Disjunction types.
     */
    Predicate firstOperand() const;

    /*!
     * A smaller, inner predicate which is the second to appear and is compared
     * with the first one in a comparison operation.
     *
     * \since 4.4
     * \note This is only valid for Conjunction and Disjunction types.
     */
    Predicate secondOperand() const;

    /*!
     * Returns if the interface type check is negated.
     * \since 6.17
     */
    bool isInterfaceTypeNegated() const;

private:
    class Private;
    Private *const d;
};
}

#endif
